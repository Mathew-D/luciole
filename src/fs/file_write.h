#pragma once

#include "fs/background_job.h"
#include "fs/location.h"

#include <string_view>

namespace luciole {
  // Creates a new file holding `bytes` at `destination`; an existing name fails with FileErrorKind::Exists.
  // `done` runs on the calling thread's main context with the created location.
  void createFileWithBytes(
      const Location& destination, std::string_view bytes, std::move_only_function<void(JobResult<Location>)> done
  );
} // namespace luciole
