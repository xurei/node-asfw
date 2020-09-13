#include "node_asfw.h"

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitAll) {
    Nan::Set(target, Nan::New("SetForegroundWindow").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(_SetForegroundWindow)).ToLocalChecked());
    Nan::Set(target, Nan::New("SetForegroundWindowByName").ToLocalChecked(),
        Nan::GetFunction(Nan::New<FunctionTemplate>(_SetForegroundWindowByName)).ToLocalChecked());
}

NODE_MODULE(asfw, InitAll)
