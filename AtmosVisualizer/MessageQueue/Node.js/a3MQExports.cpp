#include <node.h>

#include "a3MessageQueueIPC.h"

using namespace v8;

// -------------------------------------Utils-------------------------------------
// global message queue storage
static a3MessageQueueIPC* mq = NULL;

#define ReturnBegin(returnResult, returnValueCount)         Isolate* isolate = Isolate::GetCurrent();\
                                                            \
                                                            Local<Array> returned = Array::New(isolate, returnValueCount);\
                                                            \
                                                            returned->Set(0, ReturnResult::pack(isolate, &returnResult));

#define Return2Begin(secondArg)                 ReturnResult returnResult; \
                                                ReturnBegin(returnResult, 2);\
                                                returned->Set(1, secondArg);

#define Return1Begin(returnResult)              ReturnBegin(returnResult, 1);

#define ReturnEnd()                             args.GetReturnValue().Set(returned);

#define return1Error(args, errMsg)              {\
                                                std::string msg("a3MQExports::");\
                                                msg += __FUNCTION__;\
                                                msg += "(): ";\
                                                msg += errMsg;\
                                                return1Error_(args, msg.c_str());}

#define checkSystem()                           if(!checkInitialized())\
                                                {\
                                                    return1Error(args, "System not initialized");\
                                                    return;\
                                                }


template<typename T>
inline void safeDelete(T*& a)
{
    if(a != NULL)
    {
        delete a;
        a = NULL;
    }
}

// -------------------------------------declaration-------------------------------------
class ReturnResult
{
public:
    ReturnResult() :ReturnResult(true, "") {}

    ReturnResult(bool success, std::string errorMsg) :success(success), errorMsg(errorMsg) {}

    ~ReturnResult() {}

    void set(bool success, std::string errorMsg)
    {
        this->success = success;
        this->errorMsg = errorMsg;
    }

    // operation result
    bool success;

    // used when failed operation
    std::string errorMsg;

    // 转换ReturnResult->Object
    static Local<Array> pack(Isolate *isolate, const ReturnResult* result)
    {
        Local<Array> obj = Array::New(isolate);

        if(result)
        {
            // 设定属性域
            obj->Set(String::NewFromUtf8(isolate, "success"), Boolean::New(isolate, result->success));
            obj->Set(String::NewFromUtf8(isolate, "errMsg"), String::NewFromUtf8(isolate, result->errorMsg.c_str()));
        }

        return obj;
    }
};

struct msgMouse :public a3MessageEntryHead
{
public:
    msgMouse() : x(0), y(0)
    {
        type = 0;
    }

    int x, y;
};

struct msgKeyboard :public a3MessageEntryHead
{
public:
    msgKeyboard()
    {
        type = 1;
    }

    int key;
};

struct msgExit :public a3MessageEntryHead
{
public:
    msgExit()
    {
        type = 2;
    }
};

// -------------------------------------check-------------------------------------
// 检查map是否被分配
bool checkInitialized()
{
    if(mq)
        return true;
    else
        return false;
}

// -------------------------------------return 2 value with int boolean or list-------------------------------------
void return2List(const FunctionCallbackInfo<Value>& args, Local<Array> list)
{
    Return2Begin(list);

    ReturnEnd();
}

void return2Int(const FunctionCallbackInfo<Value>& args, int num)
{
    Return2Begin(Int32::New(isolate, num));

    ReturnEnd();
}

void return2Bool (const FunctionCallbackInfo<Value>& args, bool b)
{
    Return2Begin(Boolean::New(isolate, b));

    ReturnEnd();
}

// -------------------------------------return 1-------------------------------------
void return1(const FunctionCallbackInfo<Value>& args, ReturnResult returnResult)
{
    Return1Begin(returnResult);

    ReturnEnd();
}

void return1Error_(const FunctionCallbackInfo<Value>& args, const char* errMsg)
{
    // returnValue 
    ReturnResult returnResult;

    returnResult.set(false, errMsg);

    return1(args, returnResult);
}

void return1Succeed(const FunctionCallbackInfo<Value>& args)
{
    // returnValue 
    ReturnResult returnResult;

    return1(args, returnResult);
}

// -------------------------------------interfaces-------------------------------------
// Message Queue
void init(const FunctionCallbackInfo<Value>& args)
{
    if(args.Length() != 4 ||
       !args[0]->IsString() ||
       !args[1]->IsBoolean() ||
       !args[2]->IsInt32() ||
       !args[3]->IsInt32())
    {
        return1Error(args, "Parameter error");
        return;
    }

    if(!mq)
    {
        mq = new a3MessageQueueIPC();

        // convert string to mq's name
        // char to wchar
        String::Utf8Value tempString(args[0]->ToString());
        std::string temp = std::string(*tempString);
        const char* mqNameChar = temp.c_str();

        const size_t cSize = strlen(mqNameChar) + 1;
        wchar_t* mqName = new wchar_t[cSize];
        mbstowcs(mqName, mqNameChar, cSize);

        bool create = args[1]->BooleanValue();
        int msgMaxNum = args[2]->Int32Value();
        int msgMaxSize = args[3]->Int32Value();

        // init message queue
        mq->init(mqName, create, msgMaxNum, msgMaxSize);

        delete mqName;

        return1Succeed(args);
    }
    else
        return1Error(args, "Message Queue already initialized");
}

void uninit(const FunctionCallbackInfo<Value>& args)
{
    if(mq)
    {
        mq->uninit();

        safeDelete(mq);
    }

    return1Succeed(args);
}

void sendMessage(const FunctionCallbackInfo<Value>& args)
{
    checkSystem();

    return1Succeed(args);
}

void getMessage(const FunctionCallbackInfo<Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();

    checkSystem();

    char msgBuffer[1024];
    memset(msgBuffer, 0, 1024);

    a3MessageEntryHead* pMsg = (a3MessageEntryHead*) msgBuffer;

    if(!mq->dequeue(*pMsg))
    {
        return1Error(args, "Empty queue");
        return;
    }

    switch(pMsg->type)
    {
    case 0:
    {
        const msgMouse* msg = (const msgMouse*) pMsg;

        Local<Array> messageObj = Array::New(isolate);
        messageObj->Set(String::NewFromUtf8(isolate, "msgType"), String::NewFromUtf8(isolate, "mouse"));
        messageObj->Set(String::NewFromUtf8(isolate, "x"), Int32::New(isolate, msg->x));
        messageObj->Set(String::NewFromUtf8(isolate, "y"), Int32::New(isolate, msg->y));

        return2List(args, messageObj);

        break;
    }

    case 1:
    {
        const msgKeyboard* msg = (const msgKeyboard*) pMsg;

        Local<Array> messageObj = Array::New(isolate);
        messageObj->Set(String::NewFromUtf8(isolate, "msgType"), String::NewFromUtf8(isolate, "keyboard"));
        // int to char
        char aChar = '0' + msg->key;
        messageObj->Set(String::NewFromUtf8(isolate, "key"), String::NewFromUtf8(isolate, &aChar));

        return2List(args, messageObj);
        break;
    }

    case 2:
    {
        const msgExit* msg = (const msgExit*) pMsg;

        Local<Array> messageObj = Array::New(isolate);
        messageObj->Set(String::NewFromUtf8(isolate, "msgType"), String::NewFromUtf8(isolate, "exit"));

        return2List(args, messageObj);
        break;
    }
    default:
        return1Error(args, "Wrong type of message");
    }
}

void isEmpty(const FunctionCallbackInfo<Value>& args)
{
    checkSystem();

    return2Bool(args, mq->isEmpty());
}

void isFull(const FunctionCallbackInfo<Value>& args)
{
    checkSystem();

    return2Bool(args, mq->isFull());
}

void getSize(const FunctionCallbackInfo<Value>& args)
{
    checkSystem();

    return2Int(args, mq->getSize());
}

void InitExports(Handle<Object> exports)
{
    NODE_SET_METHOD(exports, "init", init);
    NODE_SET_METHOD(exports, "uninit", uninit);
    NODE_SET_METHOD(exports, "sendMessage", sendMessage);
    NODE_SET_METHOD(exports, "getMessage", getMessage);
    NODE_SET_METHOD(exports, "isEmpty", isEmpty);
    NODE_SET_METHOD(exports, "isFull", isFull);
    NODE_SET_METHOD(exports, "getSize", getSize);
}

NODE_MODULE(mq, InitExports)