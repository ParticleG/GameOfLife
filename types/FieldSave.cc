#include <types/FieldSave.h>

using namespace std;
using namespace types;

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
            "{:%H:%M:%S}",
            _saveTime
        )
    };
}

bool FieldSave::isValid() const {
    return !_field.empty();
}

tuple<Field, NeightborRule, NeightborRule> FieldSave::load() const {
    return {_field, _aliveRule, _deadRule};
}

void FieldSave::reset() {
    _field = Field();
    _name.clear();
}

void FieldSave::save(const Field& field, const NeightborRule aliveRule, const NeightborRule deadRule) {
    _saveTime = chrono::system_clock::now();
    _field = field;
    _aliveRule = aliveRule;
    _deadRule = deadRule;
}
