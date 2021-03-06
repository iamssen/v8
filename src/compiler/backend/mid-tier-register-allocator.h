// Copyright 2020 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_BACKEND_MID_TIER_REGISTER_ALLOCATOR_H_
#define V8_COMPILER_BACKEND_MID_TIER_REGISTER_ALLOCATOR_H_

#include "src/base/compiler-specific.h"
#include "src/common/globals.h"
#include "src/compiler/backend/instruction.h"
#include "src/compiler/backend/register-allocation.h"
#include "src/flags/flags.h"
#include "src/utils/bit-vector.h"
#include "src/zone/zone-containers.h"
#include "src/zone/zone.h"

namespace v8 {
namespace internal {

class TickCounter;

namespace compiler {

class VirtualRegisterData;

// The MidTierRegisterAllocator is a register allocator specifically designed to
// perform register allocation as fast as possible while minimizing spill moves.

class MidTierRegisterAllocationData final : public RegisterAllocationData {
 public:
  MidTierRegisterAllocationData(const RegisterConfiguration* config,
                                Zone* allocation_zone, Frame* frame,
                                InstructionSequence* code,
                                TickCounter* tick_counter,
                                const char* debug_name = nullptr);

  static MidTierRegisterAllocationData* cast(RegisterAllocationData* data) {
    DCHECK_EQ(data->type(), Type::kMidTier);
    return static_cast<MidTierRegisterAllocationData*>(data);
  }

  VirtualRegisterData& VirtualRegisterDataFor(int virtual_register);
  MachineRepresentation RepresentationFor(int virtual_register);

  // List of all instruction indexs that require a reference map.
  ZoneVector<int>& reference_map_instructions() {
    return reference_map_instructions_;
  }

  // This zone is for data structures only needed during register allocation
  // phases.
  Zone* allocation_zone() const { return allocation_zone_; }

  // This zone is for InstructionOperands and moves that live beyond register
  // allocation.
  Zone* code_zone() const { return code()->zone(); }

  InstructionSequence* code() const { return code_; }
  Frame* frame() const { return frame_; }
  const char* debug_name() const { return debug_name_; }
  const RegisterConfiguration* config() const { return config_; }
  TickCounter* tick_counter() { return tick_counter_; }

 private:
  Zone* const allocation_zone_;
  Frame* const frame_;
  InstructionSequence* const code_;
  const char* const debug_name_;
  const RegisterConfiguration* const config_;

  ZoneVector<VirtualRegisterData> virtual_register_data_;
  ZoneVector<int> reference_map_instructions_;

  TickCounter* const tick_counter_;

  DISALLOW_COPY_AND_ASSIGN(MidTierRegisterAllocationData);
};

class MidTierRegisterAllocator final {
 public:
  explicit MidTierRegisterAllocator(MidTierRegisterAllocationData* data);
  ~MidTierRegisterAllocator();

  // Phase 1: Process instruction outputs to determine how each virtual register
  // is defined.
  void DefineOutputs();

  // TODO(rmcilroy): Phase 2 - allocate registers to instructions.

 private:
  // Define outputs operations.
  void InitializeBlockState(const InstructionBlock* block);
  void DefineOutputs(const InstructionBlock* block);

  VirtualRegisterData& VirtualRegisterDataFor(int virtual_register) const {
    return data()->VirtualRegisterDataFor(virtual_register);
  }
  MachineRepresentation RepresentationFor(int virtual_register) const {
    return data()->RepresentationFor(virtual_register);
  }
  MidTierRegisterAllocationData* data() const { return data_; }
  InstructionSequence* code() const { return data()->code(); }
  Zone* allocation_zone() const { return data()->allocation_zone(); }

  MidTierRegisterAllocationData* data_;

  DISALLOW_COPY_AND_ASSIGN(MidTierRegisterAllocator);
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_BACKEND_MID_TIER_REGISTER_ALLOCATOR_H_
