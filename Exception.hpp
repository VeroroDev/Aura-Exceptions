#include <cstdint>
#include <cstdlib>

namespace Aura
{
    using u32 = std::uint32_t;

    namespace Meta
    {
        template<class T> class RemoveReferencesClass
        {
        public:
            using Type = T;
        };

        template<class T> class RemoveReferencesClass<T&>
        {
        public:
            using Type = T;
        };

        template<class T> class RemoveReferencesClass<T&&>
        {
        public:
            using Type = T;
        };

        template<class T> using RemoveReferences = typename RemoveReferencesClass<T>::Type;
    }

    class ThrowableHandler
    {
    public:
        u32 ID;

        [[nodiscard]] inline constexpr ThrowableHandler() noexcept = default;

        [[nodiscard]] inline constexpr ThrowableHandler(const u32 id) noexcept : ID(id){}

        [[nodiscard]] inline constexpr ThrowableHandler(const ThrowableHandler& other) noexcept : ID(other.ID){}

        [[nodiscard]] inline constexpr bool operator==(const ThrowableHandler& other) const noexcept
        {
            return other.ID == ID;
        }

        [[nodiscard]] inline constexpr bool operator==(const u32 id) const noexcept
        {
            return id == ID;
        }
    };

    template<class T> class Throwable
    {
        using R = Meta::RemoveReferences<T>;

        union ThrowableInternal
        {
            R obj;
    
            ThrowableHandler th;
        };

        ThrowableInternal ti;
    public:
        bool abort = false;

        [[nodiscard]] inline constexpr Throwable() noexcept = default;

        [[nodiscard]] inline constexpr Throwable(const ThrowableHandler& e) noexcept
        {
            ti.th = e;
        }

        [[nodiscard]] inline constexpr Throwable(R obj) noexcept : ti(obj){}

        template<class... U> [[nodiscard]] inline constexpr bool Catch(ThrowableHandler et, U&&... ets) const noexcept
        {
            if(ti.th == et)
                return true;
            else if constexpr(sizeof...(ets) > 0ull)
                return Catch(ets...);
            else if(!(ti.th == 0ul) && abort)
                std::abort();
            else
                return false;
        }

        [[nodiscard]] inline constexpr bool Any() const noexcept
        {
            return !ti.th == 0ul;
        }

        inline constexpr void operator=(const ThrowableHandler& e) noexcept
        {
            ti.th = e;
        }

        [[nodiscard]] inline constexpr R& operator()() noexcept
        {
            return ti.obj;
        }
    };

    class DefaultThrowable{};
}

#define no_throw(...) return throwable<__VA_ARGS__>();
#define throw return
#define catch(x, ...) if(x.Catch(__VA_ARGS__))
#define any(x) if(x.Any())
template<class T = Aura::DefaultThrowable> using throwable = Aura::Throwable<T>;
using error = Aura::ThrowableHandler;
constexpr error no_error(0ul);
