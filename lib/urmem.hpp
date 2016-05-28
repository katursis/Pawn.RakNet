#ifndef URMEM_H_
#define URMEM_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <sys/stat.h> 
#include <sys/mman.h>
#include <unistd.h>
#endif

#include <vector>
#include <iterator>
#include <algorithm>
#include <memory>
#include <mutex>

// TODO: x64 support

class urmem
{
public:

	using address_t = unsigned long;
	using byte_t = unsigned char;
	using bytearray_t = std::vector<byte_t>;

	enum class calling_convention
	{
		cdeclcall,
		stdcall,
		thiscall
	};

	template<calling_convention CConv, typename Ret = void, typename ... Args>
	static Ret call_function(address_t address, Args ... args)
	{
#ifdef _WIN32
		return invoker<CConv>::call<Ret, Args...>(address, args...);
#else
		return (reinterpret_cast<Ret(*)(Args...)>(address))(args...);
#endif
	}

	template<typename T>
	static address_t get_func_addr(T func)
	{
		union
		{
			T func;
			address_t addr;
		} u{ func };

		return u.addr;
	};

	template<typename T>
	class bit_manager
	{
	public:

		bit_manager(void) = delete;
		bit_manager(T &src) :_data(src) {}

		class bit
		{
		public:

			bit(void) = delete;
			bit(T &src, size_t index) : _data(src), _mask(1 << index), _index(index) {}

			bit &operator=(bool value)
			{
				if (value)
					_data |= _mask;
				else
					_data &= ~_mask;

				return *this;
			}

			operator bool() const
			{
				return (_data & _mask) != 0;
			}

		private:

			T &_data;
			const T _mask;
			const size_t _index;
		};

		bit operator [](size_t index)
		{
			return bit(_data, index);
		};

	private:

		T &_data;
	};

	class pointer
	{
	public:

		pointer(void) = delete;
		template<typename T>
		pointer(T *address) : pointer{ reinterpret_cast<address_t>(address) } {}
		pointer(address_t address) : _pointer(address) {}

		template<typename T>
		T &field(size_t offset)
		{
			return *reinterpret_cast<T *>(_pointer + offset);
		}

		pointer ptr_field(size_t offset)
		{
			return pointer(field<address_t>(offset));
		}

		template<typename T>
		operator T *() const
		{
			return reinterpret_cast<T *>(_pointer);
		}

	private:

		const address_t _pointer;
	};

	class unprotect_scope
	{
	public:

		unprotect_scope(void) = delete;
		unprotect_scope(address_t addr, size_t length) :_addr(addr), _lenght(length)
		{
#ifdef _WIN32
			VirtualProtect(reinterpret_cast<void *>(_addr), _lenght, PAGE_EXECUTE_READWRITE, &_original_protect);
#else
			auto pagesize = sysconf(_SC_PAGE_SIZE);

			_addr = _addr & ~(pagesize - 1);

			mprotect(reinterpret_cast<void *>(_addr), _lenght, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
		}

		~unprotect_scope(void)
		{
#ifdef _WIN32
			VirtualProtect(reinterpret_cast<void *>(_addr), _lenght, _original_protect, nullptr);
#else
			mprotect(reinterpret_cast<void *>(_addr), _lenght, PROT_READ | PROT_EXEC);
#endif
		}

	private:
#ifdef _WIN32
		unsigned long _original_protect;
#endif
		address_t _addr;
		const size_t _lenght;
	};

	class sig_scanner
	{
	public:

		bool init(void *addr_in_module) { return init(reinterpret_cast<address_t>(addr_in_module)); }
		bool init(address_t addr_in_module)
		{
#ifdef _WIN32					
			MEMORY_BASIC_INFORMATION info{};
			if (!VirtualQuery(reinterpret_cast<void *>(addr_in_module), &info, sizeof(info)))
				return false;

			auto dos = reinterpret_cast<IMAGE_DOS_HEADER *>(info.AllocationBase);
			auto pe = reinterpret_cast<IMAGE_NT_HEADERS *>(reinterpret_cast<address_t>(dos) + dos->e_lfanew);

			if (pe->Signature != IMAGE_NT_SIGNATURE)
				return false;

			_base = reinterpret_cast<address_t>(info.AllocationBase);
			_size = pe->OptionalHeader.SizeOfImage;
#else	
			Dl_info info{};
			struct stat buf {};

			if (!dladdr(reinterpret_cast<void *>(addr_in_module), &info))
				return false;

			if (stat(info.dli_fname, &buf) != 0)
				return false;

			_base = reinterpret_cast<address_t>(info.dli_fbase);
			_size = buf.st_size;
#endif
			return true;
		}

		bool find(const char *pattern, const char *mask, address_t &addr) const
		{
			auto current_byte = reinterpret_cast<byte_t *>(_base);
			auto last_byte = current_byte + _size;

			size_t i{};
			while (current_byte < last_byte)
			{
				for (i = 0; mask[i]; ++i)
				{
					if ((mask[i] != '?') && (static_cast<byte_t>(pattern[i]) != current_byte[i]))
						break;
				}

				if (!mask[i])
				{
					addr = reinterpret_cast<address_t>(current_byte);

					return true;
				}

				++current_byte;
			}

			return false;
		}

	private:

		address_t _base{};
		size_t _size{};
	};

	class patch
	{
	public:

		patch(void) = delete;
		patch(void *addr, const bytearray_t &new_data)
			: patch{ reinterpret_cast<address_t>(addr), new_data } {}
		patch(address_t addr, const bytearray_t &new_data)
			: _patch_addr(addr), _new_data(new_data), _enabled(false)
		{
			enable();
		}

		~patch(void)
		{
			disable();
		}

		void enable(void)
		{
			if (_enabled)
				return;

			unprotect_scope scope(_patch_addr, _new_data.size());

			_original_data.clear();

			std::copy_n(reinterpret_cast<bytearray_t::value_type*>(_patch_addr), _new_data.size(),
				std::back_inserter<bytearray_t>(_original_data)); // save original bytes

			std::copy_n(_new_data.data(), _new_data.size(),
				reinterpret_cast<bytearray_t::value_type*>(_patch_addr)); // put new bytes	

			_enabled = true;
		}

		void disable(void)
		{
			if (!_enabled)
				return;

			unprotect_scope scope(_patch_addr, _new_data.size());

			std::copy_n(_original_data.data(), _original_data.size(),
				reinterpret_cast<bytearray_t::value_type*>(_patch_addr)); // put original bytes			

			_enabled = false;
		}

		bool is_enabled(void) const
		{
			return _enabled;
		}

	private:

		address_t _patch_addr;
		bytearray_t _original_data;
		bytearray_t _new_data;
		bool _enabled;
	};

	class hook
	{
	public:

		enum class type
		{
			jmp,
			call
		};

		class raii
		{
		public:

			raii(void) = delete;
			raii(hook &h) : _hook(h) { _hook.disable(); }

			~raii(void) { _hook.enable(); }

		private:

			hook &_hook;
		};

		hook(void) = delete;
		hook(void *inject_addr, void *handle_addr, hook::type h_type = hook::type::jmp, size_t length = 5) :
			hook{ reinterpret_cast<address_t>(inject_addr), reinterpret_cast<address_t>(handle_addr), h_type, length } {};
		hook(address_t inject_addr, address_t handle_addr, hook::type h_type = hook::type::jmp, size_t length = 5)
		{
			bytearray_t new_bytes(length, 0x90);

			switch (h_type)
			{
				case type::jmp:
				{
					new_bytes[0] = 0xE9;
					_original_addr = inject_addr;
					break;
				}
				case type::call:
				{
					new_bytes[0] = 0xE8;
					_original_addr = pointer(inject_addr).field<address_t>(1) + (inject_addr + 5);
					break;
				}
			}

			*reinterpret_cast<address_t *>(new_bytes.data() + 1) = handle_addr - (inject_addr + 5);

			_patch = std::make_shared<patch>(inject_addr, new_bytes);
		}

		void enable(void)
		{
			_patch->enable();
		}

		void disable(void)
		{
			_patch->disable();
		}

		bool is_enabled(void) const
		{
			return _patch->is_enabled();
		}

		address_t get_original_addr(void) const
		{
			return _original_addr;
		}

	private:

		address_t _original_addr{};
		std::shared_ptr<patch> _patch;
	};

	template<size_t, calling_convention, typename Sig>
	class smart_hook;

	template<size_t Id, calling_convention CConv, typename Ret, typename ... Args>
	class smart_hook<Id, CConv, Ret(Args...)>
	{
	public:

		using func = std::function<Ret(Args...)>;

		smart_hook(void *inject_addr, hook::type h_type = hook::type::jmp, size_t length = 5) :
			smart_hook{ reinterpret_cast<address_t>(inject_addr), h_type, length } {};
		smart_hook(address_t inject_addr, hook::type h_type = hook::type::jmp, size_t length = 5)
		{
			get_data() = this;

			_hook = std::make_shared<hook>(inject_addr, reinterpret_cast<address_t>(_interlayer.func), h_type, length);
		}

		void attach(const func &f)
		{
			_cb = f;
		}

		void detach(void)
		{
			_cb = nullptr;
		}

		Ret call(Args ... args)
		{
			return call_function<CConv, Ret>(_hook->get_original_addr(), args...);
		}

	private:

		static smart_hook<Id, CConv, Ret(Args...)> *&get_data(void)
		{
			static smart_hook<Id, CConv, Ret(Args...)> *d{};

			return d;
		}
#ifdef _WIN32
		template<calling_convention>
		struct interlayer;

		template<>
		struct interlayer<calling_convention::cdeclcall>
		{
			static Ret __cdecl func(Args ... args)
			{
				return get_data()->call_cb(args...);
			}
		};

		template<>
		struct interlayer<calling_convention::stdcall>
		{
			static Ret __stdcall func(Args ... args)
			{
				return get_data()->call_cb(args...);
			}
		};

		template<>
		struct interlayer<calling_convention::thiscall>
		{
			static Ret __thiscall func(Args ... args)
			{
				return get_data()->call_cb(args...);
			}
		};
#else
		struct interlayer
		{
			static Ret func(Args ... args)
			{
				return get_data()->call_cb(args...);
			}
		};
#endif

		inline Ret call_cb(Args ... args)
		{
			std::lock_guard<std::mutex> guard(_mutex);

			hook::raii scope(*_hook);

			return _cb ? _cb(args...) : call(args...);
		}

		std::shared_ptr<hook> _hook;
		std::mutex _mutex;
#ifdef _WIN32
		interlayer<CConv> _interlayer;
#else
		interlayer _interlayer;
#endif
		func _cb;
	};

private:
#ifdef _WIN32
	template<calling_convention>
	struct invoker;

	template<>
	struct invoker<calling_convention::cdeclcall>
	{
		template<typename Ret, typename ... Args>
		static inline Ret call(address_t address, Args... args)
		{
			return (reinterpret_cast<Ret(__cdecl *)(Args...)>(address))(args...);
		}
	};

	template<>
	struct invoker<calling_convention::stdcall>
	{
		template<typename Ret, typename ... Args>
		static inline Ret call(address_t address, Args... args)
		{
			return (reinterpret_cast<Ret(__stdcall *)(Args...)>(address))(args...);
		}
	};

	template<>
	struct invoker<calling_convention::thiscall>
	{
		template<typename Ret, typename ... Args>
		static inline Ret call(address_t address, Args... args)
		{
			return (reinterpret_cast<Ret(__thiscall *)(Args...)>(address))(args...);
		}
	};
#endif	
};

#endif // URMEM_H_