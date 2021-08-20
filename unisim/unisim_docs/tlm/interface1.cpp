template <typename REQ, typename RSP>
class TlmSendIf : public virtual sc_interface
{
public:
	virtual bool Send(TlmMessage<REQ, RSP> &message) = 0;
};

template <typename REQ, typename RSP>
class TlmMessage
{
public:
	REQ req;
	RSP rsp;
	sc_event event;
};
