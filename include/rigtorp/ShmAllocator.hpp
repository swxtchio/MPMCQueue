#pragma once

// clang-format off
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#undef max
#endif
// clang-format on

#include <string>

#include <iostream>

namespace rigtorp {
namespace shm {

enum class Mode_e { ALLOCATE, CONNECT };

class ShmAllocator_t {
private:
  std::string m_Name;
  Mode_e m_Mode;
  uint8_t *m_StartOfShm = nullptr;
  HANDLE m_MapFileHandle = INVALID_HANDLE_VALUE;

public:
  ShmAllocator_t() = default;
  ShmAllocator_t(const std::string &name, const Mode_e mode)
      : m_Name(name), m_Mode(mode) {}
  ShmAllocator_t &operator=(const ShmAllocator_t &) = delete;

  uint8_t *allocateShm(int bufSize) {
    m_MapFileHandle =
        CreateFileMapping(INVALID_HANDLE_VALUE, // use paging file
                          NULL,                 // default security
                          PAGE_READWRITE,       // read/write access
                          0,       // maximum object size (high-order DWORD)
                          bufSize, // maximum object size (low-order DWORD)
                          m_Name.data()); // name of mapping object
    m_StartOfShm =
        (uint8_t *)MapViewOfFile(m_MapFileHandle,     // handle to map object
                                 FILE_MAP_ALL_ACCESS, // read/write permission
                                 0, 0, bufSize);
    if (m_StartOfShm == NULL) {
      CloseHandle(m_MapFileHandle);
      throw std::runtime_error("Could not map view of file: " +
                               std::to_string(GetLastError()));
    }
    return m_StartOfShm;
  }

  uint8_t *connectToShm(int bufSize) {
    m_MapFileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, // read/write access
                                      FALSE,          // do not inherit the name
                                      m_Name.data()); // name of mapping object

    if (m_MapFileHandle == INVALID_HANDLE_VALUE) {
      throw std::runtime_error("Could not open file mapping object: " +
                               std::to_string(GetLastError()));
    }

    m_StartOfShm =
        (uint8_t *)MapViewOfFile(m_MapFileHandle,     // handle to map object
                                 FILE_MAP_ALL_ACCESS, // read/write permission
                                 0, 0, bufSize);
    if (m_StartOfShm == NULL) {
      CloseHandle(m_MapFileHandle);
      throw std::runtime_error("Could not map view of file: " +
                               std::to_string(GetLastError()));
    }
    return m_StartOfShm;
  }

  uint8_t *allocate(int bufSize) {
    if (m_Mode == Mode_e::ALLOCATE) {
      return allocateShm(bufSize);
    } else {
      return connectToShm(bufSize);
    }
  }

  void deallocate() {
    if (m_StartOfShm != nullptr) {
      UnmapViewOfFile(m_StartOfShm);
      m_StartOfShm = nullptr;
    }
    if (m_MapFileHandle != INVALID_HANDLE_VALUE) {
      CloseHandle(m_MapFileHandle);
      m_MapFileHandle = INVALID_HANDLE_VALUE;
    }
  }

  const Mode_e Mode() const noexcept { return m_Mode; }

  const std::string Name() const  noexcept{ return m_Name; }

  ~ShmAllocator_t() { deallocate(); }
};

} // namespace shm
} // namespace rigtorp
