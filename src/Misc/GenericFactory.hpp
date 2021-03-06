#ifndef GENERIC_FACTORY_HPP
#define GENERIC_FACTORY_HPP

#include <map>
#include <any>
#include <functional>
#include <stdexcept>

#include "TypeName.hpp"
#include "CallableTraits.hpp"
#include "BaseFactoryObject.hpp"

/**
 * Обобщённый фактори.
 *
 * Позволяет регистрировать типы, производные от Base для последующего создания.
 */

template<class Base = BaseFactoryObject>
class GenericFactory {
  public:
    /**
     *  Регистрация Дефолтного, производного от Base типа
     *  Дефолтный тип имеется ввиду прямой, не производный тип
     */

    template<typename T, typename... CreatorArgs>
    void registerDefaultType() {
        return registerType<T, T, CreatorArgs...>();
    }

    /**
     *  Регистрация Производного от Base типа
     */

    template<typename T, typename Derived, typename... CreatorArgs>
    requires std::derived_from<Derived, T>
    void registerType() {
        typedef typename MakeCreator<CreatorArgs...>::TCreator TCreator;

        TCreator creator{[] (auto &&... args) -> T * {
                return new Derived(std::forward<CreatorArgs>(args)...);
            }
        };

        insert <T>(std::move(creator));
    }

    /**
     *  Регистрация Производного от Base типа
     *  сюда передаём лямбду, простой функтор или указатель на функцию
     *  Callable c является конструктором, который должа возвратить новый инстанс типа
     */

    template<CallablePointerResultDerived<Base> Callable>
    void registerType(Callable &&c) {
        typedef typename
        MakeCreator<typename CallableTraits<Callable>::args_tuple>::TCreator TCreator;
        TCreator creator{std::move(c)};
        using RetType = typename
                        std::remove_pointer<typename CallableTraits<Callable>::result_type>::type;

        insert <RetType>(std::move(creator));
    }

    /**
     *  Создание эксземпляра типа.
     *
     *  Если тут лезет std::any bad_cast exception,
     *  то явно укажите типы аргумента конструктора, например:
     *  было так: create<T1>(arg0), стало так:
     *  create<T1, const T2&>(arg0)
     */

    template<class T, typename... CreatorArgs>
    requires std::derived_from<T, Base>
    T *create(CreatorArgs&&... args) {
        const std::string className(getTypeName<T>());

        if (m_types.find(className) == m_types.end())
            throw std::runtime_error("Type does not registered");

        typedef typename MakeCreator<CreatorArgs...>::TCreator TCreator;
        std::any &c = m_types[className];
        TCreator &creator = std::any_cast<TCreator &>(c);
        return static_cast<T *>(creator(std::forward<CreatorArgs>(args)...));
    }

  private:
    template<typename... Args>
    struct MakeCreator {
        typedef std::function<Base*(Args...)> TCreator;
    };

    template<typename... Ts>
    struct MakeCreator<std::tuple<Ts...>> {
        typedef std::function<Base*(Ts...)> TCreator;
    };

    template<class KeyType, class Creator>
    void insert(Creator &&creator) {
        const std::string className(getTypeName<KeyType>());
        m_types.erase(className);
        m_types.emplace(className, std::move(creator));
    }

    std::map<std::string, std::any> m_types;
};

#endif // GENERIC_FACTORY_HPP
