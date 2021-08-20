#include <unisim/util/hypapp/hypapp.hh>

#include <iostream>
#include <sstream>

#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

char const* defaultPages[]={"/index.htm","/index.html","/default.htm","/main.xul"};

namespace {
  void SanitizeURIPath( std::string& sanitized, char const* uri )
  {
    sanitized = "/";
    
    for (;;)
      {
        while (*uri == '/') ++uri;
        
        if (not *uri) break;
        
        if (uri[0] == '.')
          {
            if (uri[1] == '.' and (uri[2] == '/' or uri[2] == '\0'))
              {
                uri += 2;
                if (sanitized != "/")
                  sanitized.erase(sanitized.rfind('/', sanitized.size() - 2)+1);
                continue;
              }
            if (uri[1] == '/' or uri[1] == '\0')
              {
                uri += 1;
                continue;
              }
          }
        
        if (char const* eoc = strchr(uri, '/'))
          {
            sanitized.append(uri, ++eoc);
            uri = eoc;
            continue;
          }
        
        // Last component wrap-up; no trailing slash and finish
        sanitized.append( uri );
        break;
      }
  }

}

struct MiniWebServer : public unisim::util::hypapp::HttpServer
{
  std::string webdocs;
  
  static unsigned const ip_port = 12347;
  
  MiniWebServer()
    : HttpServer(ip_port,32), webdocs(".")
  {}

  void Kill() { killed = true; }
  bool IsRunning() const { return running; }
  
  virtual void Serve(unisim::util::hypapp::ClientConnection const& conn) override
  {
    struct MiniWebMsgLoop : public unisim::util::hypapp::MessageLoop
    {
      MiniWebMsgLoop(MiniWebServer& _http_server)
        : unisim::util::hypapp::MessageLoop(_http_server, std::cerr, std::cerr, std::cerr)
      {}
      MiniWebServer& GetServer() { return static_cast<MiniWebServer&>(http_server); }
      
      bool SendNotFound(const unisim::util::hypapp::ClientConnection& conn, char const* path)
      {
        std::ostringstream pagebuf;
        pagebuf << "<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>"
                << path
                << " not found.</p></body></html>";
        std::string page( pagebuf.str() );
        std::ostringstream header;
        header << "HTTP/1.1 404 Not Found\r\nServer: miniweb\r\nContent-length: " << page.size()
               << "\r\nContent-Type: text/html\r\n\r\n";
        conn.Send( header.str() );
        conn.Send( page );
        
        return true;
      }
      
      virtual bool SendDirectory(const unisim::util::hypapp::ClientConnection& conn, std::string const& path, std::string const& rl)
      {
        std::ostringstream pagebuf;
        pagebuf << "<html><head><title>" << rl << "</title></head>"
          "<body><table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"100%\">"
            << "<h2>Directory of /" << rl << "</h2><hr>";

        std::cerr << "listing directory " << path << std::endl;
    
        struct DirIterator
        {
          DirIterator( char const* dirpath ) : curdir( OpenDir( dirpath ) ), dirent(0) {}
          ~DirIterator() { if (curdir) closedir(curdir); }
          bool next() { return curdir and (dirent = readdir(curdir)); }
          char const* GetName() const { return dirent->d_name; }
      
          static DIR* OpenDir( char const* dirpath )
          {
            struct stat dirstat;
            if ((stat( dirpath, &dirstat) < 0) or not (dirstat.st_mode & S_IFDIR))
              return 0;
            return opendir( dirpath );
          }
          DIR* curdir;
          struct dirent* dirent;
        };
    
        for (DirIterator itr(path.c_str()); itr.next();)
          {
            char const* filename = itr.GetName();
            if (strcmp(filename, ".") == 0)
              continue;
            std::cerr << "Checking " << filename << " ...\n";
            std::string filepath( path );
            filepath = filepath + "/" + filename;
        
            struct stat st;
            if (stat(filepath.c_str(),&st))
              continue;
          
            bool is_dir = (st.st_mode & S_IFDIR);
            pagebuf << "<tr><td width=\"35%\"><a href='" << filename
                << (is_dir ? "/" : "") << "'>" << filename
                << "</a></td><td width=\"15%\">";
        
            if (is_dir)
              pagebuf << "&lt;dir&gt;</td><td width=\"15%\">";
            else
              {
                pagebuf << uintptr_t(st.st_size >> 10) << " KB</td><td width=\"15%\">";
                if (char const* ext = strrchr(filename,'.'))
                  pagebuf << ext << " file";
              }
        
            {
              std::ostringstream tmbuf;
              time_t timer = st.st_mtime;
              struct tm* btm = gmtime(&timer);
              char const* dayNames="Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat";
              char const* monthNames="Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec";
              tmbuf << &dayNames[btm->tm_wday<<2] << ", "
                    << btm->tm_mday << ' '
                    << &monthNames[btm->tm_mon<<2] << ' '
                    << (1900+btm->tm_year) << ' '
                    << btm->tm_hour << ':'
                    << btm->tm_min << ':'
                    << btm->tm_sec;
              pagebuf << "</td><td>" << tmbuf.str() << "</td></tr>";
            }
          }
    
        pagebuf << "</table><hr><i>Directory content generated by HypApp</i></body></html>";
        
        std::string page( pagebuf.str() );
        
        std::ostringstream header;
        header << "HTTP/1.1 200 OK\r\nServer: miniweb\r\nCache-control: no-cache\r\n"
          "Connection: keep-alive\r\nContent-length: " << page.size()
               << "\r\nContent-Type: text/html\r\n\r\n";
        conn.Send( header.str() );
        conn.Send( page );
    
        return true;
      }
      
      virtual bool SendResponse(const unisim::util::hypapp::Request& req, const unisim::util::hypapp::ClientConnection& conn) override
      {
        std::string path;
        SanitizeURIPath( path, req.GetRequestURI() );
        
        path.insert( 0, GetServer().webdocs );
        
        struct stat st;
        if (stat(path.c_str(),&st) < 0)
          return SendNotFound( conn, req.GetRequestURI() );
        
        int fd = -1;
        if (st.st_mode & S_IFDIR)
          {
            std::cerr << "Process Directory...\n";
            if (path[path.size()-1] != '/')
              path += '/';
            // first try opening default pages
            for (int i=0; i<(int)(sizeof(defaultPages)/sizeof(defaultPages[0])); i++) {
              std::string subpath( path + defaultPages[i] );
              if ((fd = open(subpath.c_str(),O_RDONLY)) >= 0)
                break;
            }
            if (fd < 0)
              return SendDirectory(conn, path.substr(0, path.size()-1), path.substr(GetServer().webdocs.size(), path.size()-1));
          }
        else
          {
            fd = open(path.c_str(), O_RDONLY);
          }
        
        if (fd >= 0)
          fstat(fd, &st);
        
        uintptr_t fileSize = st.st_size;
        
        conn.Send( "HTTP/1.1 200 OK\r\nServer: miniweb\r\nCache-control: no-cache\r\nConnection: keep-alive\r\n" );
       
        // TODO: handle CSEQ
        
        std::string ext = path.substr(path.find_last_of( "/." ));
        
        if      (ext == ".htm" or ext == ".html")
          conn.Send( "Content-Type: text/html\r\n" );
        else if (ext == ".css")
          conn.Send( "Content-Type: text/css\r\n" );
        else if (ext == ".js")
          conn.Send( "Content-Type: application/javascript\r\n" );
        else
          conn.Send( "Content-Type: application/octet-stream\r\n" );
        
        {
          std::ostringstream buf;
          buf << "Content-Length: " << fileSize << "\r\n\r\n";
          conn.Send( buf.str() );
        }
        
        {
          char buf[0x10000];
        
          for (uintptr_t done; fileSize > 0; fileSize -= done)
            {
              done = read(fd, &buf[0], sizeof (buf));
              if ((done > sizeof (buf)) or (done > 0))
                break;
              conn.Send( &buf[0], done );
            }
        
          for (;fileSize > sizeof (buf); fileSize -= sizeof (buf))
            conn.Send(buf);
        
          if (fileSize > 0)
            conn.Send(&buf[0],fileSize);
        }
        
        return true;
      }
      
    //   std::string const& webdocs;
    //   char const* uri;
    };

    MiniWebMsgLoop mwml(*this);
    mwml.Run(conn);
  }
};

struct SignalHandling
{
  static MiniWebServer* miniweb;
  
  static void Target( MiniWebServer& mw )
  {
    miniweb = &mw;
    signal(SIGINT, (sighandler_t) AbortServer);
    signal(SIGTERM, (sighandler_t) AbortServer);
    signal(SIGPIPE, SIG_IGN);
  }
  
  static int AbortServer(int arg)
  {
    if (arg)
      std::cerr << "\nCaught signal (" << arg << "). MiniWeb shutting down...\n";
    if (not miniweb)
      throw 0;
    miniweb->Kill();
    return 0;
  }
};

int
main(int argc,char* argv[])
{
  // Taken from {MiniWeb by Stanley Huang <stanleyhuangyc@gmail.com>};
  
  MiniWebServer miniweb;
  
  SignalHandling::Target( miniweb );
  
  std::cout << "Host: 127.0.0.1:" << miniweb.ip_port << "\n"
            << "Web root: " << miniweb.webdocs << "\n";
  
  // Run server
  miniweb.StartLoopThread();
  
  while (miniweb.IsRunning())
    sleep(1);
  
  return 0;
}

MiniWebServer* SignalHandling::miniweb = 0;
