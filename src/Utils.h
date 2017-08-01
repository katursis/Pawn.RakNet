#ifndef UTILS_H_
#define UTILS_H_

namespace Utils {
    inline bool check_params(const char *native, int count, cell *params) {
        if (params[0] != (count * sizeof(cell))) {
            Logger::instance()->Write("[%s] %s: invalid number of parameters. Should be %d", Settings::kPluginName, native, count);

            return false;
        }

        return true;
    }

    inline int set_amxstring(AMX *amx, cell amx_addr, const char *source, int max) {
        cell *dest = reinterpret_cast<cell *>(
            amx->base + static_cast<int>(reinterpret_cast<AMX_HEADER *>(amx->base)->dat + amx_addr)
            );

        cell *start = dest;

        while (max-- && *source) {
            *dest++ = static_cast<cell>(*source++);
        }

        *dest = 0;

        return dest - start;
    }

    inline bool get_public_var(AMX *amx, const char *name, cell &out) {
        cell addr{},
            *phys_addr{};

        if ((amx_FindPubVar(amx, name, &addr) == AMX_ERR_NONE) &&
            (amx_GetAddr(amx, addr, &phys_addr) == AMX_ERR_NONE)) {
            out = *phys_addr;

            return true;
        }

        return false;
    }
}

#endif  // UTILS_H_
