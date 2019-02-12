#ifndef LOGGER_H_
#define LOGGER_H_

class Logger : public Singleton<Logger> {
public:
    using logprintf_t = void(*)(const char *format, ...);

    void Init(void *logprintf) {
        _logprintf = reinterpret_cast<logprintf_t>(logprintf);
    }

    template<typename ... ARGS>
    void Write(const std::string &fmt, ARGS ... args) {
        if (!_logprintf) {
            throw std::runtime_error{"logger was not initialized"};
        }

        _logprintf(fmt.c_str(), args...);
    }

private:
    friend class Singleton<Logger>;

    Logger() : _logprintf{nullptr} {}

    logprintf_t _logprintf;
};

#endif // LOGGER_H_
