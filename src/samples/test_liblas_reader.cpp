// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#if defined(_MSC_VER) && defined(USE_VLD)
#include <vld.h>
#endif
// liblas
#include <liblas/liblas.hpp>
#include <liblas/point.hpp>
#include <liblas/reader.hpp>
#include <liblas/detail/timer.hpp>
//std
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <utility>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <iterator>
#include <cstdlib>
#include <cassert>
// sample
#include <liblas/utility.hpp>
using namespace std;

int main()
{
	try
	{
		//char const* name = "d:\\data\\lidar\\LASFile_1.LAS";
		//char const* name = "d:\\data\\lidar\\LDR030828_213450_0.LAS"; 
		//char const* name = "d:\\data\\lidar\\Sample_LiDAR_LAS_File.las"; // 1.1
		//char const* name = "d:\\data\\lidar\\Serpent_Mound_Model.las";
		//char const* name = "d:\\data\\lidar\\gilmer\\000001.las";
		//char const* name = "d:\\data\\lidar\\iowa\\04164492.las";        
		//char const* name = "d:\\dev\\liblas\\_svn\\trunk\\test\\data\\TO_core_last_clip.las";
		char const* name = "D:/Data/las_data/waveform/full_waveform_tutorial_data-Milton Keynes201207.LAS";

		std::ifstream ifs;
		if (!liblas::Open(ifs, name))
		{
			throw std::runtime_error(std::string("Can not open ") + name);
		}
		liblas::Reader reader(ifs);
		liblas::Header const& h = reader.GetHeader();

		cout << "File name: " << name << '\n';
		cout << "Version  : " << (int)h.GetVersionMajor() << "." << (int)h.GetVersionMinor()<< '\n';
		cout << "Signature: " << h.GetFileSignature() << '\n';
		cout << "Format   : " << h.GetDataFormatId() << '\n';
		cout << "Project  : " << h.GetProjectId() << '\n';
		cout << "Points count: " << h.GetPointRecordsCount() << '\n';
		cout << "VLRecords count: " << h.GetRecordsCount() << '\n';
		cout << "Points by return: ";
		std::copy(h.GetPointRecordsByReturnCount().begin(),
			h.GetPointRecordsByReturnCount().end(),
			ostream_iterator<uint32_t>(std::cout, " "));
		cout << std::endl;

		liblas::detail::Timer t;
		t.start();

		typedef std::pair<double, double> minmax_t;
		minmax_t mx;
		minmax_t my;
		minmax_t mz;
		uint32_t i = 0;
		while (reader.ReadNextPoint())
		{
			liblas::Point const& p = reader.GetPoint();

			mx.first = std::min<double>(mx.first, p[0]);
			mx.second = std::max<double>(mx.second, p[0]);
			my.first = std::min<double>(my.first, p[1]);
			my.second = std::max<double>(my.second, p[1]);
			mz.first = std::min<double>(mz.first, p[2]);
			mz.second = std::max<double>(mz.second, p[2]);

			// Warning: Printing zillion of points may take looong time
			cout << i << ". " << p << '\n';
			i++;
			if( i == 2)
				break;
		}
		double const d = t.stop();

		uint32_t j = 0;
		reader.Reset();
		while (reader.ReadWaveAt(j))
		{
			const std::vector<uint8_t>& p = reader.GetWave();

			std::cout << j << ": " << (int)p[0] << " " << (int)p[1] << " " << (int)p[2] << std::endl;
			j++;
			if (j == 2)
			{
				break;
			}
		}
	}
	catch (std::exception const& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown error\n";
	}

	return 0;
}
