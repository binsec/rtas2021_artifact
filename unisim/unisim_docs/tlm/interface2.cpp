template <typename REQ, typename RSP>
class TlmSendIf : public virtual sc_interface
{
public:
	virtual bool Send(const Pointer<TlmMessage<REQ, RSP> > &message) = 0;
};

template <typename REQ, typename RSP>
class TlmMessage
{
public:
	Pointer<REQ> req;
	Pointer<RSP> rsp;
	sc_event event;
};
