#include "tasks/paramonov_leonid_broadcast/seq/include/ops_seq.hpp"

namespace paramonov_leonid_broadcast {

template <typename T>
ParamonovLeonidBroadcastSEQ<T>::ParamonovLeonidBroadcastSEQ(const InType &in) {
  this->SetTypeOfTask(GetStaticTypeOfTask());
  this->GetInput() = in;
  this->GetOutput().resize(0);
}

template <typename T>
bool ParamonovLeonidBroadcastSEQ<T>::ValidationImpl() {
  return (!this->GetInput().empty()) && (this->GetOutput().empty());
}

template <typename T>
bool ParamonovLeonidBroadcastSEQ<T>::PreProcessingImpl() {
  return true;
}

template <typename T>
bool ParamonovLeonidBroadcastSEQ<T>::RunImpl() {
  this->GetOutput() = this->GetInput();
  return true;
}

template <typename T>
bool ParamonovLeonidBroadcastSEQ<T>::PostProcessingImpl() {
  return true;
}

template class ParamonovLeonidBroadcastSEQ<int>;
template class ParamonovLeonidBroadcastSEQ<float>;
template class ParamonovLeonidBroadcastSEQ<double>;

}  // namespace paramonov_leonid_broadcast
