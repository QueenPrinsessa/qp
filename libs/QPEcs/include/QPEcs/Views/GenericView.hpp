#pragma once
#include "QPEcs/Entity.hpp"
#include <unordered_set>

namespace QPEcs
{
	class EntityComponentSystem;

	class GenericView
	{
		friend class ViewManager;
		public:
			virtual ~GenericView() = default;

			size_t Size() const;
			bool Empty() const;
			bool HasEntities() const;

			Entity GetFirstEntity() const;

			std::unordered_set<Entity>::iterator begin();
			std::unordered_set<Entity>::iterator end();
			std::unordered_set<Entity>::const_iterator begin() const;
			std::unordered_set<Entity>::const_iterator end() const;
		protected:
			std::unordered_set<Entity> myEntities {};
			EntityComponentSystem* myECS { nullptr };
	};

	inline size_t GenericView::Size() const
	{
		return myEntities.size();
	}

	inline bool GenericView::Empty() const
	{
		return (myEntities.size() == 0);
	}

	inline bool GenericView::HasEntities() const
	{
		return myEntities.size() != 0;
	}

	inline Entity GenericView::GetFirstEntity() const
	{
		if(myEntities.begin() != myEntities.end())
		{
			return *myEntities.begin();
		}
		else
		{
			return QPEcs::NullEntity;
		}
	}

	inline std::unordered_set<Entity>::iterator GenericView::begin()
	{
		return myEntities.begin();
	}

	inline std::unordered_set<Entity>::iterator GenericView::end()
	{
		return myEntities.end();
	}

	inline std::unordered_set<Entity>::const_iterator GenericView::begin() const
	{
		return myEntities.begin();
	}

	inline std::unordered_set<Entity>::const_iterator GenericView::end() const
	{
		return myEntities.end();
	}
}
