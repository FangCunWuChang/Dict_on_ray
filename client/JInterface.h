#pragma once
#include <JuceHeader.h>

//接口管理器，用于保存模块注册的接口

namespace jmadf {
	namespace inside {
		class CallBackObjectBase
		{
		public:
			CallBackObjectBase() = default;
			virtual ~CallBackObjectBase() = default;
		};

		template<typename ...T>
		class CallBackObject final :
			public CallBackObjectBase
		{
			using F = std::function<void(T...)>;
			const F _data;
			const char* const _typename;
			const F _empty = [](T...) {};

		public:
			explicit CallBackObject(const F& data)
				:_data(data), _typename(typeid(F).name())
			{};

			~CallBackObject() override = default;

			const F& operator()()
			{
				if (strcmp(this->_typename, typeid(F).name())) {
					jassertfalse;//Interface args type isn't matched!
					return this->_empty;
				}
				return this->_data;
			};

			const char* type()
			{
				return this->_typename;
			};

		private:
			JUCE_LEAK_DETECTOR(CallBackObject<T...>)
		};

		template<>
		class CallBackObject<void> final :
			public CallBackObjectBase
		{
			using F = std::function<void(void)>;
			const F _data;
			const char* const _typename;
			const F _empty = [] {};

		public:
			explicit CallBackObject(const F& data)
				:_data(data), _typename(typeid(F).name())
			{};

			~CallBackObject() override = default;

			const F& operator()()
			{
				if (strcmp(this->_typename, typeid(F).name())) {
					jassertfalse;//Interface args type isn't matched!
					return this->_empty;
				}
				return this->_data;
			};

			const char* type()
			{
				return this->_typename;
			};

		private:
			JUCE_LEAK_DETECTOR(CallBackObject<void>)
		};
	}
	
	class JInterface final
	{
	public:
		static JInterface* getInstance() 
		{
			if (!JInterface::_instance)
			{
				JInterface::_instance = std::make_unique<JInterface>();
			}
			return JInterface::_instance.get();
		};
		static void destoryInstance() 
		{
			JInterface::_instance = nullptr; 
		};

	private:
		static std::unique_ptr<JInterface> _instance;

	public:
		JInterface() = default;
		~JInterface()
		{
			this->_lock.enterWrite();
			this->list.clear();
			this->_lock.exitWrite();
		};

	public:
		juce::OwnedArray<inside::CallBackObjectBase> _memory;
		juce::HashMap<juce::String, inside::CallBackObjectBase*> list;
		juce::ReadWriteLock _lock;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JInterface)
	};
	
	template<typename ...T>
	class JInterfaceDao final
	{
		using F = std::function<void(T...)>;
		using U = inside::CallBackObject<T...>;

		JInterfaceDao() = delete;
		~JInterfaceDao() = delete;

	public:
		static void set(const juce::String& key, const F& func)
		{
			JInterface* instance = JInterface::getInstance();
			instance->_lock.enterWrite();
			inside::CallBackObjectBase* obj = new U(func);
			instance->_memory.add(obj);
			instance->list.set(
				key,
				obj
			);
			instance->_lock.exitWrite();
		};

		static void call(const juce::String& key, T ...args)
		{
			JInterface* instance = JInterface::getInstance();
			instance->_lock.enterRead();
			if (instance->list.contains(key)) {
				U* obj = reinterpret_cast<U*>(instance->list[key]);
				const F& func = (*obj)();
				func(args...);
			}
			else {
				jassertfalse;//Interface isn't exists!
			}
			instance->_lock.exitRead();
		};
	};

	template<>
	class JInterfaceDao<void> final
	{
		using F = std::function<void(void)>;
		using U = inside::CallBackObject<void>;

		JInterfaceDao() = delete;
		~JInterfaceDao() = delete;

	public:
		static void set(const juce::String& key, const F& func)
		{
			JInterface* instance = JInterface::getInstance();
			instance->_lock.enterWrite();
			inside::CallBackObjectBase* obj = new U(func);
			instance->_memory.add(obj);
			instance->list.set(
				key,
				obj
			);
			instance->_lock.exitWrite();
		};

		static void call(const juce::String& key)
		{
			JInterface* instance = JInterface::getInstance();
			instance->_lock.enterRead();
			if (instance->list.contains(key)) {
				U* obj = reinterpret_cast<U*>(instance->list[key]);
				const F& func = (*obj)();
				func();
			}
			else {
				jassertfalse;//Interface isn't exists!
			}
			instance->_lock.exitRead();
		};
	};
}
