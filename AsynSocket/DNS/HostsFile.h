#ifndef _HOSTSFILE_H_269A39BA_E2CF_4CED_BD12_4F4F384202D7
#define _HOSTSFILE_H_269A39BA_E2CF_4CED_BD12_4F4F384202D7


#include <string>
#include <map>
namespace DNS
{
	class HostsFile
	{
	public:
		HostsFile();
		~HostsFile();
		/* 查询 */
		bool Query(std::string& const domain, std::string& ip);
		/* 读取hosts文件，及C:\Windows\System32\drivers\etc\hosts */
		void Init();
	private:
		void ParseLine(std::string& const strLine);
		/* 相同域名只取第一个 */
		std::map<std::string, std::string> m_mapNameIp;
	};
}

#endif