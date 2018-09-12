class AppBase
{
public:
	AppBase(){};
	virtual ~AppBase(){};
    virtual void Init() = 0;
    virtual bool Run() = 0;
    virtual void Release() = 0;
};