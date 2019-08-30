#include "myYaml/include/myYaml/node/emit.h"
#include "myYaml/include/myYaml/emitfromevents.h"
#include "myYaml/include/myYaml/emitter.h"
#include "nodeevents.h"

namespace myYAML {
Emitter& operator<<(Emitter& out, const Node& node) {
  EmitFromEvents emitFromEvents(out);
  NodeEvents events(node);
  events.Emit(emitFromEvents);
  return out;
}

std::ostream& operator<<(std::ostream& out, const Node& node) {
  Emitter emitter(out);
  emitter << node;
  return out;
}

std::string Dump(const Node& node) {
  Emitter emitter;
  emitter << node;
  return emitter.c_str();
}
}  // namespace myYAML
