// exception classes for various error types

#ifndef myExceptions_
#define myExceptions_
#include <string>

using namespace std;

// illegal parameter value
class illegalParameterValue 
{
   public:
      illegalParameterValue(string theMessage = "Illegal parameter value")
            {message = theMessage;}
      void outputMessage() {cout << message << endl;}
   private:
      string message;
};

// queue is empty
class queueEmpty
{
   public:
      queueEmpty(string theMessage = 
                   "Invalid operation on empty queue")
            {message = theMessage;}
      void outputMessage() {cout << message << endl;}
   private:
      string message;
};

#endif
