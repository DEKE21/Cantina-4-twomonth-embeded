#include "Utils/Datastructure.hpp"
//#include <filesystem>
#include <fstream>
#include <string>

namespace Telemetry {

class CSVFileWriter {
private:
  std::ofstream file;
  //std::filesystem::path save_dir = "D:/";

public:
  CSVFileWriter();


  void WriteData(TelemetryData data);

  void StopWriting() {
    if (file.is_open()) {
      file.close();
    }
  }
  void StartWriting();
};

} // namespace Telemetry
