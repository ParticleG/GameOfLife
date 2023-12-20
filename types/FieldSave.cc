#include <types/FieldSave.h>

using namespace std;
using namespace types;

FieldSave::FieldSave(string name, Field field)
    : _saveTime(chrono::system_clock::now()), _name(move(name)), _field(move(field)) {}

void FieldSave::changeName(const string& name) {
    _name = name;
}

string FieldSave::getName() const {
    return _name;
}

tuple<string, string> FieldSave::getSaveTime() const {
    return {
        format("{:%Y/%m/%d}", _saveTime),
        format(
            "{:%H:%M:%S}.{:03d}",
            _saveTime,
            chrono::duration_cast<chrono::milliseconds>(_saveTime.time_since_epoch()).count() % 1000
        )
    };
}

Field FieldSave::load() const {
    return _field;
}

void FieldSave::save(const Field& field) {
    _field = field;
    _saveTime = chrono::system_clock::now();
}
