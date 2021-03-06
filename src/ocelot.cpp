#include "ocelot.h"
#include "config.h"
#include "db.h"
#include "worker.h"
#include "events.h"
#include "schedule.h"
#include "site_comm.h"
#include "logger.h"
#include "cache.h"

static connection_mother *mother;
static worker *work;
struct stats_t stats;

static void sig_handler(int sig) {
  Logger::warn("Caught SIGINT/SIGTERM");
  if (work->signal(sig)) {
    exit(0);
  }
}

int ocelot_main(int argc, char **argv) {
  // we don't use printf so make cout/cerr a little bit faster
  std::ios_base::sync_with_stdio(false);
  
  Logger::set_log_level( LogLevel::INFO );

  signal(SIGINT, sig_handler);
  signal(SIGTERM, sig_handler);

  bool verbose = false;
  for (int i = argc; i > 1; i--) {
    if (!strcmp(argv[1], "-v")) {
      verbose = true;
    }
  }

  auto db = mysql::get_instance();
  
  if (!db->connected()) {
    Logger::fail("Cannot connect to database!");
    return 0;
  }
  db->m_verbose_flush = verbose;
  
  config conf;

  site_comm sc;
  sc.verbose_flush = verbose;

  std::vector<std::string> whitelist;
  db->load_whitelist(whitelist);
  
  Logger::info("Loaded " + std::to_string( whitelist.size() ) + " clients into the whitelist");
  
  if (whitelist.size() == 0) {
    Logger::info("Assuming no whitelist desired, disabling");
  }

  user_list users_list;
  db->load_users(users_list);
  
  Logger::info("Loaded " + std::to_string( users_list.size() ) + " users");

  torrent_list torrents_list;
  db->load_torrents(torrents_list);
  Logger::info("Loaded " + std::to_string( torrents_list.size() ) + " torrents");

  db->load_tokens(torrents_list);

  stats.open_connections = 0;
  stats.opened_connections = 0;
  stats.connection_rate = 0;
  stats.leechers = 0;
  stats.seeders = 0;
  stats.announcements = 0;
  stats.succ_announcements = 0;
  stats.scrapes = 0;
  stats.bytes_read = 0;
  stats.bytes_written = 0;
  stats.start_time = time(NULL);
  
  // Set Cache
  TorrentListCache::set( torrents_list );
  UserListCache::set( users_list );
  WhitelistCache::set( whitelist );

  // Create worker object, which handles announces and scrapes and all that jazz
  work = new worker(torrents_list, users_list, whitelist, &conf, &sc);

  // Create connection mother, which binds to its socket and handles the event stuff
  mother = new connection_mother(work);

  return 0;
}
