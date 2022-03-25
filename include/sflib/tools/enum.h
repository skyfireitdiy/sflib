_Pragma("once");

#include "cocpp/core/co_define.h"
#include <any>
#include <functional>

CO_NAMESPACE_BEGIN

template <typename... DataList> class sf_enum_item {
private:
  std::tuple<DataList...> data;

public:
  sf_enum_item(DataList... data_list);
  void apply(std::function<void(DataList... data_list)> f);
};

template <typename... DataList>
sf_enum_item(DataList...) -> sf_enum_item<DataList...>;

template <typename Target, typename... ItemType>
concept ValidEnumType = (false || ... || std::is_same_v<Target, ItemType>);

template <typename... ItemTypes> class sf_enum {
private:
  std::any data__;

  template <typename ItemType, typename... Args>
  sf_enum &match__(std::function<void(Args...)> f);

public:
  template <typename U>
  requires ValidEnumType<U, ItemTypes...> sf_enum(const U &item);

  template <typename ItemType, typename U>
  requires ValidEnumType<ItemType, ItemTypes...> sf_enum &match(U f);
};

#define MAKE_ENUM_ITEM(Name, ...)                                              \
  class Name final : public sf_enum_item<__VA_ARGS__> {                        \
  public:                                                                      \
    using sf_enum_item<__VA_ARGS__>::sf_enum_item;                             \
  };

#define MAKE_ENUM(Name, ...) using Name = sf_enum<__VA_ARGS__>;

//////////////////////////////////////////////////////////////////
template <typename... DataList>
sf_enum_item<DataList...>::sf_enum_item(DataList... data_list)
    : data(std::make_tuple(data_list...)) {}
template <typename... DataList>
void sf_enum_item<DataList...>::apply(
    std::function<void(DataList... data_list)> f) {
  std::apply(f, data);
}

template <typename... ItemTypes>
template <typename ItemType, typename... Args>
sf_enum<ItemTypes...> &
sf_enum<ItemTypes...>::match__(std::function<void(Args...)> f) {
  if (data__.type() == typeid(ItemType)) {
    std::any_cast<ItemType>(data__).apply(f);
  }
  return *this;
}

template <typename... ItemTypes> template <typename U>
requires ValidEnumType<U, ItemTypes...>
sf_enum<ItemTypes...>::sf_enum(const U &item) : data__(item) {}

template <typename... ItemTypes> template <typename ItemType, typename U>
requires ValidEnumType<ItemType, ItemTypes...> sf_enum<ItemTypes...>
&sf_enum<ItemTypes...>::match(U f) {
  return match__<ItemType>(std::function(f));
}
CO_NAMESPACE_END