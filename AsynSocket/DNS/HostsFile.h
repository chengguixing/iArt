#ifndef _HOSTSFILE_H_269A39BA_E2CF_4CED_BD12_4F4F384202D7
#define _HOSTSFILE_H_269A39BA_E2CF_4CED_BD12_4F4F384202D7


#include <map>
namespace DNS
{
	/* hosts */
	class HostsFile
	{
	public:
		HostsFile();
		~HostsFile();
		/* 查询 */
		bool Query(const std::string& domain, std::string& ip);
		/* 读取hosts文件，及C:\Windows\System32\drivers\etc\hosts */
		void Init();
	private:
		void ParseLine(const std::string& strLine);
		/* 相同域名只取第一个 */
		std::map<std::string, std::string> m_mapNameIp;
	};
}

#endif