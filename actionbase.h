#ifndef  ACTIONBASE_H
#define  ACTIONBASE_H

namespace ui {
	class ActionBase {
		public :
			virtual ~ActionBase() {};
			virtual void init() = 0;
	};
}



#endif  /*ACTIONBASE_H*/
