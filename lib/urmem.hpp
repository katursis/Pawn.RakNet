#ifndef URMEM_H_
#define URMEM_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h> 
#endif

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>
#include <iterator>
#include <string>
#include <memory>

namespace urmem
{
	typedef unsigned long		address_t;
	typedef unsigned char		byte_t;
	typedef std::vector<byte_t>	bytearray_t;	

	enum class calling_convention
	{
		cdeclcall,
		stdcall,
		thiscall
	};

	class memory
	{		
	public:

		class pointer
		{
		public:

			template<typename T>
			inline pointer(T *address) : _pointer(reinterpret_cast<address_t>(address)) {}
			inline pointer(const address_t &address) : _pointer(address) {}
			inline pointer(void) : _pointer(0) {}

			template<typename T>
			inline T &field(unsigned int offset)
			{
				return *(reinterpret_cast<T*>(_pointer + offset));
			}

			inline pointer ptr_field(unsigned int offset)
			{
				return pointer(field<address_t>(offset));
			}

			template<typename T>
			inline operator T*()
			{
				return reinterpret_cast<T*>(_pointer);
			}

		private:
			address_t _pointer;
		};

#ifdef _WIN32	
		class unprotect_context
		{
		public:

			inline unprotect_context(address_t addr, size_t length) :_addr(addr), _lenght(length)
			{
				VirtualProtect(reinterpret_cast<byte_t*>(_addr), _lenght, PAGE_EXECUTE_READWRITE, &_original_protect);
			}

			inline ~unprotect_context(void)
			{
				VirtualProtect(reinterpret_cast<byte_t*>(_addr), _lenght, _original_protect, nullptr);
			}

		private:

			unsigned long	_original_protect;
			address_t		_addr;
			size_t			_lenght;
		};
#else
		class unprotect_context
		{
		public:

			inline unprotect_context(address_t addr, size_t length) :_addr(addr), _lenght(length)
			{
				auto pagesize = sysconf(_SC_PAGE_SIZE);

				_addr = _addr & ~(pagesize - 1);

				mprotect(reinterpret_cast<void *>(_addr), _lenght, PROT_READ | PROT_WRITE | PROT_EXEC);
			}

			inline ~unprotect_context(void)
			{
				mprotect(reinterpret_cast<void *>(_addr), _lenght, PROT_READ | PROT_EXEC);
			}

		private:

			address_t		_addr;
			size_t			_lenght;
		};
#endif

		static bool find_address(const char *pattern, const char *mask, urmem::address_t &address)
		{
			void *addr{};
			std::size_t size{};
#ifdef _WIN32	
			addr = GetModuleHandleA(nullptr);

			MEMORY_BASIC_INFORMATION info{};

			if (!VirtualQuery(addr, &info, sizeof(info)))
				return false;

			auto dos = reinterpret_cast<IMAGE_DOS_HEADER *>(info.AllocationBase);
			auto pe = reinterpret_cast<IMAGE_NT_HEADERS *>(reinterpret_cast<urmem::address_t>(dos) + dos->e_lfanew);

			if (pe->Signature != IMAGE_NT_SIGNATURE)
				return false;

			size = pe->OptionalHeader.SizeOfImage;
#else
			Dl_info info;
			struct stat buf;

			if (!dladdr(addr, &info))
				return false;

			if (!info.dli_fbase || !info.dli_fname)
				return false;

			if (stat(info.dli_fname, &buf) != 0)
				return false;

			addr = info.dli_fbase;
			size = buf.st_size;
#endif
			auto current_byte = reinterpret_cast<urmem::byte_t *>(addr);
			auto last_byte = current_byte + size;

			size_t i{};
			while (current_byte < last_byte)
			{
				for (i = 0; mask[i]; ++i)
				{
					if ((mask[i] != '?') && (static_cast<urmem::byte_t>(pattern[i]) != current_byte[i]))
						break;
				}

				if (!mask[i])
				{
					address = reinterpret_cast<urmem::address_t>(current_byte);

					return true;
				}

				++current_byte;
			}

			return false;
		}

		template<typename returnType = void, typename... argTypes>
		inline static returnType call_function(const calling_convention convention, urmem::address_t address, argTypes... args)
		{
#ifdef _WIN32
			switch (convention)
			{
				case calling_convention::cdeclcall: 
					return (reinterpret_cast<returnType(__cdecl *)(argTypes...)>(address))(args...);
				case calling_convention::stdcall: 
					return (reinterpret_cast<returnType(__stdcall *)(argTypes...)>(address))(args...);
				case calling_convention::thiscall: 
					return (reinterpret_cast<returnType(__thiscall *)(argTypes...)>(address))(args...);
			}

			return returnType();
#else
			return (reinterpret_cast<returnType(*)(argTypes...)>(address))(args...);
#endif
		}
	};
				
	class patch
	{
	public:	
		
		inline static std::shared_ptr<patch> create(const std::string &name, const address_t &addr, const bytearray_t &new_data)
		{
			if (patch::is_exists(name))
				return nullptr;

			return patch::get_map()[name] = std::make_shared<patch>(addr, new_data);
		}

		inline static std::shared_ptr<patch> get(const std::string &name)
		{
			std::map<std::string, std::shared_ptr<patch>>::iterator it;

			if (!patch::is_exists(name, it))
				return nullptr;

			return it->second;
		}

		inline static bool is_exists(const std::string &name, std::map<std::string, std::shared_ptr<patch>>::iterator &it)
		{
			return (it = patch::get_map().find(name)) != patch::get_map().end();
		}

		inline static bool is_exists(const std::string &name)
		{
			return patch::get_map().find(name) != patch::get_map().end();
		}		

		patch(void) = delete;
		patch(const patch&) = delete;
		patch &operator=(patch&) = delete;

		patch(const address_t &addr, const bytearray_t &new_data)
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

			memory::unprotect_context context(_patch_addr, _new_data.size());			 
			
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

			memory::unprotect_context context(_patch_addr, _new_data.size());

			std::copy_n(_original_data.data(), _original_data.size(),
				reinterpret_cast<bytearray_t::value_type*>(_patch_addr)); // put original bytes			

			_enabled = false;
		}

		inline static std::map<std::string, std::shared_ptr<patch>> &get_map(void)
		{
			static std::map<std::string, std::shared_ptr<patch>> patch_map;

			return patch_map;
		}

	private:	

		address_t		_patch_addr;
		bytearray_t		_original_data;
		bytearray_t		_new_data;
		bool			_enabled;		
	};
	
	class hook
	{
	public:		

		enum class type
		{
			jmp,
			call
		};
		
		class context
		{
		public:

			inline context(std::shared_ptr<hook> &h) : _this(h) { _this->disable(); }
			inline context(const std::string &hookname) : _this(hook::get(hookname)) { _this->disable(); }			
			inline ~context(void) { _this->enable(); }

			template<typename returnType = void, typename... argTypes>
			inline returnType call_original(const calling_convention &convention, argTypes... args)
			{
				return memory::call_function<returnType>(convention, _this->_original_func_addr, args...);
			}

		private:

			std::shared_ptr<hook> _this;
		};
		
		inline static std::shared_ptr<hook> create(const std::string &name, address_t inject_addr, 
			address_t handle_addr, hook::type hook_t = hook::type::jmp, size_t length = 5 /*default len for hook*/)
		{
			if (hook::is_exists(name))
				return nullptr;

			return hook::get_map()[name] = std::make_shared<hook>(hook_t, inject_addr, handle_addr, length);
		}

		inline static std::shared_ptr<hook> get(const std::string &name)
		{
			std::map<std::string, std::shared_ptr<hook>>::iterator it;			

			if (!hook::is_exists(name, it))							
				return nullptr;
			
			return it->second;
		}
		
		inline static bool is_exists(const std::string &name, std::map<std::string, std::shared_ptr<hook>>::iterator &it)
		{			
			return (it = hook::get_map().find(name)) != hook::get_map().end();
		}

		inline static bool is_exists(const std::string &name)
		{
			return hook::get_map().find(name) != hook::get_map().end();
		}

		hook(void) = delete;
		hook(const hook&) = delete;
		hook &operator=(hook&) = delete;

		hook(type hook_t, address_t inject_addr, address_t handle_addr, size_t length)
			:_inject_addr(inject_addr), _original_func_addr(0)
		{
			bytearray_t new_bytes(length, 0x90);

			switch (hook_t)
			{
				case type::jmp:
				{
					new_bytes[0] = 0xE9; // jmp		
					_original_func_addr = _inject_addr;
					break;
				}
				case type::call:
				{
					new_bytes[0] = 0xE8; // call	
					_original_func_addr = 
						memory::pointer(_inject_addr).field<address_t>(1) + (_inject_addr + 5); 
					// calculate addr of called function
					
					break;
				}
			}

			memory::pointer(new_bytes.data()).field<address_t>(1) = 
				handle_addr - (_inject_addr + 5); // calculate offset on handle

			_patch = std::make_shared<patch>(_inject_addr, new_bytes);
		}

		inline void enable(void) { _patch->enable(); }

		inline void disable(void) { _patch->disable(); }

		inline static std::map<std::string, std::shared_ptr<hook>> &get_map(void)
		{
			static std::map<std::string, std::shared_ptr<hook>> hook_map;

			return hook_map;
		}
		
	private:		
		
		address_t				_inject_addr;	
		address_t				_original_func_addr;
		std::shared_ptr<patch>	_patch;			
	};		
	
};

#endif // URMEM_H_