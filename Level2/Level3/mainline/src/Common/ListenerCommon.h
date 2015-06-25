#pragma once

// ListenerCommon.h

/*
	// Declare like this
	BEGINE_LISTENER_DECLARE()
		LISTEN_METHOD_DECLARE(void OnRefresh(int nCount))
	END_LISTENER_DECLARE()


	// Use like this
	DEAL_LISTENER_HANDLER(OnRefresh(_nYourCount));
*/



#define BEGINE_LISTENER_DECLARE()                                            \
	public:                                                                  \
		class Listener                                                       \
		{                                                                    \
		public:

#define LISTEN_METHOD_DECLARE(VirtualMethodDeclaration)                      \
			virtual VirtualMethodDeclaration = 0;

#define LISTEN_METHOD_VOID(VirtualMethodDeclaration)                      \
	virtual VirtualMethodDeclaration {};

#define END_LISTENER_DECLARE()                                               \
		};                                                                   \
		std::vector<Listener*> mListenerList;                                \
		void AddListener(Listener* lis){ mListenerList.push_back(lis); }     \
		void RemoveListener(Listener* lis) {                                 \
			std::vector<Listener*>::iterator ite =                           \
				std::find(mListenerList.begin(), mListenerList.end(), lis);  \
				if (ite != mListenerList.end()) mListenerList.erase(ite);    \
		}                                                                    \
		void ClearListeners(){ mListenerList.clear(); }	


#define DEAL_LISTENER_HANDLER(handler)        \
	for (std::vector<Listener*>::iterator     \
		ite = mListenerList.begin();          \
		ite != mListenerList.end();           \
		ite++)                                \
	{                                         \
		(*ite)->handler;                      \
	}

#define DEAL_FIRST_LISTENER(handler)          \
		mListenerList.front()->handler
