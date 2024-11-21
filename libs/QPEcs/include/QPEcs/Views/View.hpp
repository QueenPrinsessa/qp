#pragma once
#include "GenericView.hpp"
#include "QPEcs/EntityComponentSystem.hpp"
#include <tuple>
#include <functional>

namespace QPEcs
{
	template <class ... Components>
	class View : public GenericView
	{
		static_assert(sizeof...(Components) > 0, "A view can't consist of 0 components!");
		public:
			virtual ~View() override = default;

			decltype(auto) Get(Entity aEntity) const;

			void ForEach(std::function<void(Entity, Components&...)> aFunctionToRun) const;
	};

	template <class ... Components>
	decltype(auto) View<Components...>::Get(Entity aEntity) const
	{
		if constexpr(sizeof...(Components) == 1)
		{
			return myECS->GetComponent<Components...>(aEntity);
		}
		else
		{
			return std::tie(myECS->GetComponent<Components>(aEntity)...);
		}
	}

	template <class ... Components>
	void View<Components...>::ForEach(std::function<void(Entity, Components&...)> aFunctionToRun) const
	{
		for (auto entity : myEntities)
		{
			aFunctionToRun(entity, myECS->GetComponent<Components>(entity)...);
		}
	}

}
