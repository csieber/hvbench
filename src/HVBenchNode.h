#include <event2/event.h>
#include <event2/bufferevent.h>
#include <fluid/base/BaseOFConnection.hh>

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <memory>

#include <string.h>
#include <arpa/inet.h>

#include <fluid/base/BaseOFConnection.hh>
#include <fluid/OFConnection.hh>
#include <fluid/OFServer.hh>

#include "base/BaseOFClient.h"

using namespace fluid_base;



class HVBenchNode
	: private BaseOFClient,
	  public OFHandler {
public:

	typedef std::shared_ptr<HVBenchNode> ptr;

	HVBenchNode(int id,
				std::string address,
				int port,
				uint64_t datapath_id,
				const struct OFServerSettings ofsc = OFServerSettings().supported_version(0x01)) :
        BaseOFClient(id, address, port) {
        this->ofsc = ofsc;
        this->datapath_id = datapath_id;
        this->conn = NULL;
    }

	static HVBenchNode::ptr create(int id, std::string address, int port, uint64_t datapath_id);

    virtual ~HVBenchNode() {
        if (conn != NULL)
            delete conn;
    }

    virtual bool start(bool block = false) {        
        return BaseOFClient::start(block);
    }

    virtual void start_conn(){
        BaseOFClient::start_conn();
    }

    virtual void stop_conn(){
        if (conn != NULL)
            conn->close();        
    }

    virtual void stop() {
        stop_conn();
        BaseOFClient::stop();
    }

    // Implement your logic here
    virtual void connection_callback(OFConnection* conn, OFConnection::Event event_type) {};
    virtual void message_callback(OFConnection* conn, uint8_t type, void* data, size_t len);
    
    void free_data(void* data);    

protected:
    OFConnection* conn;
    uint64_t datapath_id;    

private:
    OFServerSettings ofsc;
    void base_message_callback(BaseOFConnection* c, void* data, size_t len);
    void base_connection_callback(BaseOFConnection* c, BaseOFConnection::Event event_type);
    static void* send_echo(void* arg);
};
