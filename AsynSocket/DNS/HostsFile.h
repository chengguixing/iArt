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
		/* ��ѯ */
		bool Query(const std::string& domain, std::string& ip);
		/* ��ȡhosts�ļ�����C:\Windows\System32\drivers\etc\hosts */
		void Init();
	private:
		void ParseLine(const std::string& strLine);
		/* ��ͬ����ֻȡ��һ�� */
		std::map<std::string, std::string> m_mapNameIp;
	};
}

#endif