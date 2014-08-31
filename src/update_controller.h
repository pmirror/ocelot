#ifndef __ocelot__update_controller__
#define __ocelot__update_controller__

#include "misc_functions.h"
#include "request.h"
#include "response.h"
#include "cache.h"
#include "config.h"

class UpdateController {
  public:
    UpdateController(const Request &request)
      : m_request(request), m_config( config::get_instance() ) {}
    
    static std::string on_request(const Request &request) {
      return UpdateController( request ).get_response();
    }
    
    std::string get_response();
    
  private:
    Request m_request;
  
    config * m_config;
  
    std::string before__authenticate();
  
    void change_passkey();
    void add_torrent();
    void update_torrent();
    void update_torrents();
    void delete_torrent();
    void add_token();
    void remove_token();
    void add_user();
    void update_user();
    void remove_user();
    void remove_users();
    void add_whitelist();
    void edit_whitelist();
    void remove_whitelist();
    void update_announce_interval();
    void torrent_info();
};

#endif /* defined(__ocelot__update_controller__) */