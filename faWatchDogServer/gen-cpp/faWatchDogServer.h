/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef faWatchDogServer_H
#define faWatchDogServer_H

#include <thrift/TDispatchProcessor.h>
#include "FaWatchDogServer_types.h"

namespace faWatchDogServer {

class faWatchDogServerIf {
 public:
  virtual ~faWatchDogServerIf() {}
  virtual void ServiceHeart(TagServiceHeartResponse& _return, const TagServiceHeartRequest& request) = 0;
};

class faWatchDogServerIfFactory {
 public:
  typedef faWatchDogServerIf Handler;

  virtual ~faWatchDogServerIfFactory() {}

  virtual faWatchDogServerIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(faWatchDogServerIf* /* handler */) = 0;
};

class faWatchDogServerIfSingletonFactory : virtual public faWatchDogServerIfFactory {
 public:
  faWatchDogServerIfSingletonFactory(const boost::shared_ptr<faWatchDogServerIf>& iface) : iface_(iface) {}
  virtual ~faWatchDogServerIfSingletonFactory() {}

  virtual faWatchDogServerIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(faWatchDogServerIf* /* handler */) {}

 protected:
  boost::shared_ptr<faWatchDogServerIf> iface_;
};

class faWatchDogServerNull : virtual public faWatchDogServerIf {
 public:
  virtual ~faWatchDogServerNull() {}
  void ServiceHeart(TagServiceHeartResponse& /* _return */, const TagServiceHeartRequest& /* request */) {
    return;
  }
};

typedef struct _faWatchDogServer_ServiceHeart_args__isset {
  _faWatchDogServer_ServiceHeart_args__isset() : request(false) {}
  bool request;
} _faWatchDogServer_ServiceHeart_args__isset;

class faWatchDogServer_ServiceHeart_args {
 public:

  faWatchDogServer_ServiceHeart_args() {
  }

  virtual ~faWatchDogServer_ServiceHeart_args() throw() {}

  TagServiceHeartRequest request;

  _faWatchDogServer_ServiceHeart_args__isset __isset;

  void __set_request(const TagServiceHeartRequest& val) {
    request = val;
  }

  bool operator == (const faWatchDogServer_ServiceHeart_args & rhs) const
  {
    if (!(request == rhs.request))
      return false;
    return true;
  }
  bool operator != (const faWatchDogServer_ServiceHeart_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const faWatchDogServer_ServiceHeart_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class faWatchDogServer_ServiceHeart_pargs {
 public:


  virtual ~faWatchDogServer_ServiceHeart_pargs() throw() {}

  const TagServiceHeartRequest* request;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _faWatchDogServer_ServiceHeart_result__isset {
  _faWatchDogServer_ServiceHeart_result__isset() : success(false) {}
  bool success;
} _faWatchDogServer_ServiceHeart_result__isset;

class faWatchDogServer_ServiceHeart_result {
 public:

  faWatchDogServer_ServiceHeart_result() {
  }

  virtual ~faWatchDogServer_ServiceHeart_result() throw() {}

  TagServiceHeartResponse success;

  _faWatchDogServer_ServiceHeart_result__isset __isset;

  void __set_success(const TagServiceHeartResponse& val) {
    success = val;
  }

  bool operator == (const faWatchDogServer_ServiceHeart_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const faWatchDogServer_ServiceHeart_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const faWatchDogServer_ServiceHeart_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _faWatchDogServer_ServiceHeart_presult__isset {
  _faWatchDogServer_ServiceHeart_presult__isset() : success(false) {}
  bool success;
} _faWatchDogServer_ServiceHeart_presult__isset;

class faWatchDogServer_ServiceHeart_presult {
 public:


  virtual ~faWatchDogServer_ServiceHeart_presult() throw() {}

  TagServiceHeartResponse* success;

  _faWatchDogServer_ServiceHeart_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class faWatchDogServerClient : virtual public faWatchDogServerIf {
 public:
  faWatchDogServerClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  faWatchDogServerClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void ServiceHeart(TagServiceHeartResponse& _return, const TagServiceHeartRequest& request);
  void send_ServiceHeart(const TagServiceHeartRequest& request);
  void recv_ServiceHeart(TagServiceHeartResponse& _return);
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class faWatchDogServerProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<faWatchDogServerIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (faWatchDogServerProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_ServiceHeart(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  faWatchDogServerProcessor(boost::shared_ptr<faWatchDogServerIf> iface) :
    iface_(iface) {
    processMap_["ServiceHeart"] = &faWatchDogServerProcessor::process_ServiceHeart;
  }

  virtual ~faWatchDogServerProcessor() {}
};

class faWatchDogServerProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  faWatchDogServerProcessorFactory(const ::boost::shared_ptr< faWatchDogServerIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< faWatchDogServerIfFactory > handlerFactory_;
};

class faWatchDogServerMultiface : virtual public faWatchDogServerIf {
 public:
  faWatchDogServerMultiface(std::vector<boost::shared_ptr<faWatchDogServerIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~faWatchDogServerMultiface() {}
 protected:
  std::vector<boost::shared_ptr<faWatchDogServerIf> > ifaces_;
  faWatchDogServerMultiface() {}
  void add(boost::shared_ptr<faWatchDogServerIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void ServiceHeart(TagServiceHeartResponse& _return, const TagServiceHeartRequest& request) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->ServiceHeart(_return, request);
    }
    ifaces_[i]->ServiceHeart(_return, request);
    return;
  }

};

} // namespace

#endif
