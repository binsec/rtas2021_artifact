
#ifndef __UNISIM_COMPONENT_CLM_CACHE_CACHE_CONTAINER_COUNTER_HH__
#define __UNISIM_COMPONENT_CLM_CACHE_CACHE_CONTAINER_COUNTER_HH__

#include <unisim/component/clm/cache/cache_container.h>

namespace unisim {
namespace component {
namespace clm {
namespace cache {


template < int nCacheLines,
int nLineSize,
int nAssociativity,
ReplacementPolicyType replacementPolicy,
int nPseudoLRUHistorySize,
int nCounterRefresh,
int nCounterTop,
int nCounterTreshold>
class CacheContainerCounter : public CacheContainer < nCacheLines, nLineSize, nAssociativity, replacementPolicy, nPseudoLRUHistorySize >
{
public:
  typedef CacheContainer < nCacheLines, nLineSize, nAssociativity, replacementPolicy, nPseudoLRUHistorySize > inherit;

  // Constructor
  CacheContainerCounter()
  { for (int i = 0; i < nCacheLines/nAssociativity; i++)
    { for (int j = 0; j < nAssociativity; j++)
      { ccounter[i][j]=0;
      }
    }
    refresh_counter=0;
  }

  void refreshCache()
  { refresh_counter++;
    if (refresh_counter == nCounterRefresh)
    { for (int i = 0; i < nCacheLines/nAssociativity; i++)
      { for (int j = 0; j < nAssociativity; j++)
        { if (inherit::cvalidity[i][j] && ccounter[i][j] < nCounterTop - 1)
            ccounter[i][j]++;
        }
      }
      refresh_counter = 0;
    }
  }

  void historyAccess(int _set, int _line)
  { resetCounter(_set,_line);
    inherit::historyAccess(_set,_line);
  }

  void historyRemove(int _set, int _line)
  { resetCounter(_set,_line);
    inherit::historyRemove(_set,_line);
  }

  bool getCached(int _set, int _line)
  { return !overThreshold(_set,_line);
  }

protected:
  uint32_t ccounter[nCacheLines/nAssociativity][nAssociativity];

private:
  int refresh_counter;

  void resetCounter(int _set, int _line)
  { ccounter[_set][_line] = 0;
  }

  bool overThreshold(int _set, int _line)
  { if (ccounter[_set][_line] > nCounterTreshold) return true;
    return false;
  }

};

} // end of namespace cache
} // end of namespace clm
} // end of namespace component
} // end of namespace unisim

#endif
