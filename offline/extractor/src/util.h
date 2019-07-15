#ifndef __XFEA_OFFLINE_UTIL_H
#define __XFEA_OFFLINE_UTIL_H

#include <string>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <set>

using namespace std;

namespace xfea { namespace offxtractor { 

class ADutil
{
public:
    inline static bool IsGBK1(unsigned char c)
    {
        return(c>0x81 - 1)&&(c<0xFD + 1);
    }

    inline static bool IsGBK2(unsigned char c)
    {
        return(c>0x40 - 1) && (c<0xFE + 1) && (c!=0x7F);
    }

    inline static void SafeToupper(char * p)
    {
        if (p == NULL || *p ==0)
            return;

        while (*p)
        {
            if (IsGBK1(*p) && IsGBK2(*(p+1)))
            {
                p+=2;
            }
            else
            {
                *p = toupper(*p);
                p++;
            }
        }
    }

    inline static void StringToupper(std::string& str)
    {
        if (str.empty())
            return;

        for (size_t i=0; i<str.size();)
        {
            if (IsGBK1(str[i]) && IsGBK2(str[i+1]))
            {
                i+=2;
            }
            else
            {
                str[i] = toupper(str[i]);
                i++;
            }
        }
    }
    inline static void StringTolower(std::string& str)
    {
        if (str.empty())
            return;

        for (size_t i=0; i<str.size();)
        {
            if (IsGBK1(str[i]) && IsGBK2(str[i+1]))
            {
                i+=2;
            }
            else
            {
                str[i] = tolower(str[i]);
                i++;
            }
        }
    }
    //Çå³ýÍ·Î²µÄspace,tab×Ö·û
    inline static std::string Trim(std::string& str)
    {
        std::string ret = str;
        std::string::size_type p = str.find_first_not_of(" \t\r\n\"");
        if (p == std::string::npos)
        {
            ret = "";
            return ret;
        }
        std::string::size_type q = str.find_last_not_of(" \t\r\n\"");
        ret = str.substr(p, q-p+1);
        return ret;
    }
    
    inline static char* timeFormat(const time_t& tv,
                                   const char* fmt,
                                   char* str,
                                   int len) {
        struct tm tm;
        localtime_r(&tv, &tm);
        strftime(str,len,fmt,&tm);
        return str;
    }
    inline static int Split(std::vector<std::string>& vs,
                            const string& line,
                            int col,
                            char dmt='\t')
    {
        string::size_type p=0;
        string::size_type q;
        for(int i=0;i<col;i++)
        {
            q = line.find(dmt,p);
            if(q == string::npos) if(i<col-1) return -1;
            vs[i]=line.substr(p,q-p);
            vs[i] = Trim(vs[i]);
            p = q+1;
        }
        return 0;
    }
    inline static void Split2(std::vector<std::string>& vs,
                            const string& line,
                            char dmt='\t')
    {
        string::size_type p=0;
        string::size_type q;
        vs.clear();
        for(;;)
        {
            q = line.find(dmt,p);
            string str = line.substr(p,q-p);
            str = Trim(str);
            if(!str.empty()) 
                vs.push_back(str);
            if(q == string::npos) 
                break;
            p = q+1;
        }
    }
    inline static void Split3(std::vector<std::string>& vs,
                            const string& line,
                            char dmt='\t')
    {
        if(line.empty())
            return; 
        string::size_type p=0;
        string::size_type q;
        vs.clear();
        for(;;)
        {
            q = line.find(dmt,p);
            string str = line.substr(p,q-p);
            str = Trim(str);
            vs.push_back(str);
            if(q == string::npos) 
                break;
            p = q+1;
        }
    }
    inline static void Split3(std::set<std::string>& ss,
                            const string& line,
                            char dmt='\t')
    {
        if(line.empty())
            return; 
        string::size_type p=0;
        string::size_type q;
        ss.clear();
        for(;;)
        {
            q = line.find(dmt,p);
            string str = line.substr(p,q-p);
            str = Trim(str);
            ss.insert(str);
            if(q == string::npos) break;
            p = q+1;
        }
    }

    inline static void Split3WithoutNull(std::set<std::string>& ss,
                            const string& line,
                            char dmt='\t')
    {
        if(line.empty())
            return; 
        string::size_type p=0;
        string::size_type q;
        ss.clear();
        for(;;)
        {
            q = line.find(dmt,p);
            string str = line.substr(p,q-p);
            str = Trim(str);
            if(!str.empty())
                ss.insert(str);
            if(q == string::npos) break;
            p = q+1;
        }
    }

    inline static void SplitInt(std::vector<int>& vs,
                            const string& line,
                            char dmt='\t')
    {
        string::size_type p=0;
        string::size_type q;
        vs.clear();
        for(;;)
        {
            q = line.find(dmt,p);
            string str = line.substr(p,q-p);
            str = Trim(str);
            if(!str.empty()) vs.push_back(atoi(str.c_str()));
            if(q == string::npos) break;
            p = q+1;
        }
    }
    inline static bool isIPAddr(const std::string& str)
    {
        if(str.empty()) return false;

        const char* p = str.c_str();
        char* q;
        int i;
        for(i=0; i< 4; i++)
        {
            int ip = strtol(p, &q, 10);
            if(*q != '.' && *q != '\0') return false;
            if(ip < 0 || ip > 255) return false;
            p = q+1;
        }
        if(*q != '\0') return false;
        if(i < 4) return false;
        return true;
    }
    
    inline static char* decodeURL(char * str)
    {
        bool bFlag = strchr(str,'%') != NULL;
        if(!bFlag && strchr(str,'+') == NULL)
            return str;

        size_t len = strlen( str );
        char *str1 = new char[len+1];
        memcpy( str1,str,len);
        str1[len] = 0; 

        char *p = str1;
        size_t i = 0;
        while( *p != '\0' )
        {
            if( *p == '%' )
            {
                p ++;
                if( *p >= '0' && *p <= '9' )
                    str[i] = *p - '0';
                else if( *p >= 'a' && *p <= 'f' )
                    str[i] = *p-'a'+10;
                else if( *p >= 'A' && *p <= 'F' )
                    str[i] = *p-'A'+10;
                else //Ö»ÊÇÒ»¸ö%·ûºÅºÍºóÃæµÄ×Ö·ûÃ»ÓÐ¹ØÏµ
                {
                    str[i++] = '%';
                    if (*p!='\0'&&*p!='&')
                    {
                        str[i++] = *p;
                        p ++;
                    }
                    continue;
                }
                str[i] *= 16;
                p ++;
                if( *p == '\0' )
                    break;
                if( *p >= '0' && *p <= '9' )
                    str[i++] += *p-'0';
                else if( *p >= 'a' && *p <= 'f' )
                    str[i++] += *p-'a'+10;
                else if( *p >= 'A' && *p <= 'F' )
                    str[i++] += *p-'A'+10;
                else // Ö»ÊÇÒ»¸ö%·ûºÅºÍºóÃæµÄ×Ö·ûÃ»ÓÐ¹ØÏµ
                {
                    str[i++] = '%';
                    str[i++] = *(p - 1); //Ç°Ò»¸ö×Ö·û
                    str[i++] = *p;      //Ä¿Ç°Õâ¸ö×Ö·û
                    p ++;
                    continue;
                }
            }
            else    //Èç¹ûÓÐ%ºÅËµÃ÷+ºÅÊÇ¿Õ¸ñ×ª»»µÄ£¬Èç¹ûÃ»ÓÐ%ºÅ£¬Ôò+ÊÇÓÃ»§ÊäÈëµÄ
            {
                if ((bFlag || (p>str1&&*(p-1)!=' '))&&(*p)=='+')
                    str[i++] = ' ';
                else
                    str[i++] = *p;
            }
            p ++;
        }
        delete[] str1;
        str[i] = '\0';
        return str;
    }

};

}}

#endif
