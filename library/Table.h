#ifndef PROJET_POO_LIBS_TABLE_H
#define PROJET_POO_LIBS_TABLE_H

#include <any>
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

struct DataType
{
    virtual ~DataType() = default;

    [[nodiscard]] virtual bool equal(const std::any& first, const std::any& second) const = 0;

    [[nodiscard]] virtual bool is_assignable(const std::any& e) const = 0;

    virtual std::ostream& format(std::ostream& stream, const std::any& e) const = 0;
};

template<typename T>
struct BuiltInDataType : DataType
{
    [[nodiscard]] bool equal(const std::any& first, const std::any& second) const override
    {
        try
        {
            return std::any_cast<T>(first) == std::any_cast<T>(second);
        }
        catch (const std::bad_any_cast&)
        {
            return false;
        }
    }

    [[nodiscard]] bool is_assignable(const std::any& e) const override
    {
        return typeid(T) == e.type();
    }

    std::ostream& format(std::ostream& stream, const std::any& e) const override
    {
        return stream << std::any_cast<T>(e);
    }
};

static const BuiltInDataType<std::uint64_t> DT_UINT64;
static const BuiltInDataType<std::uint8_t> DT_UINT8;
static const BuiltInDataType<std::string> DT_STRING;

struct FieldSpec
{
    const std::string name;
    const bool foreign, primary;
    const DataType& type;

    FieldSpec(std::string name, bool foreign, bool primary, const DataType& type);

    FieldSpec(const std::string& name, const DataType& type);
};

class Table
{
private:
    struct Column
    {
        const FieldSpec spec;
        std::vector<std::any> data;

        void ensureValid(const std::any& e) const;
    };

    friend class RowWriteIterator;

    std::vector<Column> columns;
    std::unordered_map<std::string, std::size_t> indices;
    std::optional<std::size_t> primaryIndex;
    std::size_t _size;

    static std::optional<std::size_t> find_index(const Column& column, const std::any& key);

    template<typename R, typename... Types, std::size_t... Is>
    R consume_row(std::size_t rowIndex, std::function<R(Types...)> func, std::index_sequence<Is...>) const
    {
        return func(std::any_cast<Types>(columns[Is].data[rowIndex])...);
    }

    std::optional<std::size_t> find_index(const std::any& primaryKey) const;

    template<typename... Types, std::size_t... Is>
    std::tuple<Types...> pack_row(std::size_t rowIndex, std::index_sequence<Is...>) const
    {
        return std::make_tuple(std::any_cast<Types>(columns[Is].data[rowIndex])...);
    }

    void removeRow(std::size_t rowIndex);

public:
    explicit Table(std::initializer_list<FieldSpec> specs);

    std::size_t size() const;

    template<std::size_t N>
    void put(const std::array<std::any, N>& elements)
    {
        if (columns.size() != N)
        {
            throw std::runtime_error("Invalid number of values.");
        }

        for (std::size_t i = 0; i < N; i++)
        {
            const std::any& e = elements[i];
            Column& column = columns[i];
            column.ensureValid(e);
            column.data.emplace_back(e);
        }

        ++_size;
    }

    template<typename... Args>
    void append(Args&&... arg)
    {
        return put(std::array<std::any, sizeof...(Args)> { arg... });
    }

    const std::vector<std::any>& column(const std::string& name) const;

    bool contains(const std::string& name, const std::any& key) const;

    bool contains(const std::any& primaryKey) const;

    void remove(const std::any& primaryKey);

    template<typename... Types>
    void removeIf(std::function<bool(Types...)> consumer)
    {
        if (columns.size() != sizeof...(Types))
        {
            throw std::runtime_error("Invalid number of types.");
        }

        for (std::size_t i = 0; i < size();)
        {
            if (consume_row<bool, Types...>(i, consumer, std::index_sequence_for<Types...> {}))
            {
                removeRow(i);
            }
            else
            {
                ++i;
            }
        }
    }

    template<typename... Types>
    std::tuple<Types...> findOrThrow(const std::any& primaryKey) const
    {
        if (columns.size() != sizeof...(Types))
        {
            throw std::runtime_error("Invalid number of types.");
        }

        std::optional<std::size_t> rowIndex = find_index(primaryKey);
        if (!rowIndex.has_value())
        {
            throw std::runtime_error("Primary key does not exist within table.");
        }

        return pack_row<Types...>(*rowIndex, std::index_sequence_for<Types...> {});
    }

    template<typename... Types>
    std::optional<std::tuple<Types...>> find(const std::any& primaryKey) const
    {
        if (columns.size() != sizeof...(Types))
        {
            throw std::runtime_error("Invalid number of types.");
        }

        std::optional<std::size_t> rowIndex = find_index(primaryKey);
        if (!rowIndex.has_value())
        {
            return std::nullopt;
        }

        return pack_row<Types...>(*rowIndex, std::index_sequence_for<Types...> {});
    }

    template<typename... Types>
    void forEach(std::function<void(Types...)> consumer) const
    {
        if (columns.size() != sizeof...(Types))
        {
            throw std::runtime_error("Invalid number of types.");
        }

        for (std::size_t i = 0; i < size(); i++)
        {
            consume_row<void, Types...>(i, consumer, std::index_sequence_for<Types...> {});
        }
    }

    template<typename... Types>
    std::vector<std::tuple<Types...>> find_all(std::function<bool(Types...)> predicate) const
    {
        if (columns.size() != sizeof...(Types))
        {
            throw std::runtime_error("Invalid number of types.");
        }

        std::vector<std::tuple<Types...>> found;
        for (std::size_t i = 0; i < size(); i++)
        {
            if (consume_row<bool, Types...>(i, predicate, std::index_sequence_for<Types...> {}))
            {
                found.emplace_back(pack_row<Types...>(i, std::index_sequence_for<Types...> {}));
            }
        }

        return found;
    }

    template<typename... Types>
    std::optional<std::tuple<Types...>> find_first(std::function<bool(Types...)> predicate) const
    {
        if (columns.size() != sizeof...(Types))
        {
            throw std::runtime_error("Invalid number of types.");
        }

        for (std::size_t i = 0; i < size(); i++)
        {
            if (consume_row<bool, Types...>(i, predicate, std::index_sequence_for<Types...> {}))
            {
                return pack_row<Types...>(i, std::index_sequence_for<Types...> {});
            }
        }

        return std::nullopt;
    }
};

#endif
