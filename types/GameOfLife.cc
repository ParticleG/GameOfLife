#include <ranges>

#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <helpers/ColorHelper.h>
#include <types/common.h>
#include <types/GameOfLife.h>
#include <utils/component.h>
#include <utils/console.h>

using namespace ftxui;
using namespace helpers;
using namespace std;
using namespace types;
using namespace utils;

namespace {
    constexpr auto cell = "██";
    constexpr auto controlPanelWidth = 35;
    constexpr auto dataPanelWidth = 25;
    constexpr auto inputWidth = 12;

    const auto controlValueSize = size(WIDTH, EQUAL, inputWidth);

    void drawCell(Canvas& c, const Point& mouse, const Color color) {
        c.DrawText(4 * mouse.x, 4 * mouse.y, cell, color);
    }
}

GameOfLife::GameOfLife(const int height, const int width)
    : _fieldManager(height, width), _fieldSize(width, height) {
    const auto [aliveRule, deadRule] = _fieldManager.getRules();
    _aliveRule = aliveRule;
    _deadRule = deadRule;
    console::setConsoleSize(height + 2, width + inputWidth * 2 + 6);
}

void GameOfLife::run() {
    auto heightString = to_string(_fieldSize.y),
            playbackIntervalString = to_string(_fieldManager.playbackInterval.load().count()),
            widthString = to_string(_fieldSize.x);

    const auto [
        heightInput,
        widthInput
    ] = _createFieldSizeInputs(heightString, widthString);
    const auto playbackIntervalInput = _createPlaybackIntervalInput(playbackIntervalString);

    auto aliveContainer = Container::Vertical({});
    auto deadContainer = Container::Vertical({});
    for (int index = 0; index <= 8; ++index) {
        CheckboxOption aliveOption, deadOption;
        aliveOption.label = format("{} cells", index);
        aliveOption.checked = &_aliveRule[index];
        aliveOption.on_change = [&, index] {
            _fieldManager.setAliveRule(index, _aliveRule[index]);
        };
        deadOption.label = format("{} cells", index);
        deadOption.checked = &_deadRule[index];
        deadOption.on_change = [&, index] {
            _fieldManager.setDeadRule(index, _deadRule[index]);
        };
        aliveContainer->Add(Checkbox(move(aliveOption)));
        deadContainer->Add(Checkbox(move(deadOption)));
    }

    auto controlButtonContainer = _createControlButtonContainer();

    const auto controlRenderer = Renderer(
        Container::Vertical({
            widthInput,
            heightInput,
            Container::Horizontal({aliveContainer, deadContainer}),
            playbackIntervalInput,
            controlButtonContainer
        }),
        [&] {
            const auto aliveCount = _fieldManager.getCellCount();
            return window(
                text("Control Panel") | hcenter | bold,
                vbox({
                    separator(),
                    hbox({
                        text("Width: ") | align_right | flex,
                        widthInput->Render() | controlValueSize,
                    }),
                    hbox({
                        text("Height: ") | align_right | flex,
                        heightInput->Render() | controlValueSize,
                    }),
                    separator(),
                    hbox({
                        aliveContainer->Render() | flex,
                        separator(),
                        deadContainer->Render() | flex,
                    }) | border | flex,
                    separator(),
                    hbox({
                        text("Interval(ms): ") | align_right | flex,
                        playbackIntervalInput->Render() | controlValueSize,
                    }),
                    controlButtonContainer->Render() | flex,
                    separator(),
                    hbox({
                        text("Iteration: ") | align_right | flex,
                        text(to_string(_fieldManager.getIteration())) | controlValueSize,
                    }),
                    hbox({
                        text("Alive: ") | align_right | flex,
                        text(to_string(aliveCount)) | controlValueSize,
                    }),
                    hbox({
                        text("Dead: ") | align_right | flex,
                        text(to_string(_fieldSize.x * _fieldSize.y - aliveCount)) | controlValueSize,
                    }),
                })
            );
        }
    );

    const auto cellEventHandler = _createCellRenderer();

    /// Use focusIndex for scroll issue, See: {@link https://github.com/ArthurSonzogni/FTXUI/issues/125}
    int focusIndex = 0, itemIndex = 0;
    auto saveListContainer = Container::Vertical({}, &itemIndex);
    for (int index = 0; index < _saveList.size(); ++index) {
        auto loadButton = component::makeButton(
            "Load",
            [&, index] {
                if (_saveList[index].isValid()) {
                    _fieldManager.isPlaying.store(false);

                    const auto [savedField, savedAliveRule, savedDeadRule] = _saveList[index].load();
                    _fieldSize = {
                        static_cast<int>(savedField.size()),
                        static_cast<int>(savedField[0].size())
                    };
                    heightString = to_string(_fieldSize.x);
                    widthString = to_string(_fieldSize.y);
                    _fieldManager.setField(savedField);
                    for (int i = 0; i < 9; ++i) {
                        _aliveRule[i] = savedAliveRule[i];
                        _deadRule[i] = savedDeadRule[i];
                    }
                    _fieldManager.setRules(_aliveRule, _deadRule);
                }
            },
            colorLightGreen
        );
        auto saveButton = component::makeButton(
            "Save",
            [&, index] {
                _saveList[index].save(_fieldManager.getField(), _aliveRule, _deadRule);
            },
            colorLightBlue
        );
        auto deleteButton = component::makeButton(
            "Delete",
            [&, index] {
                _saveList[index].reset();
            },
            colorRedLight,
            colorNegative
        );
        const auto saveItem = Renderer([&, index] {
                const auto isValid = _saveList[index].isValid();
                const auto [saveDate, saveTime] = _saveList[index].getSaveTime();
                Element element;
                if (isValid) {
                    element = vbox({
                        text(format("Slot No.{}", index + 1)),
                        vbox({
                            text(format("Name: {}", _saveList[index].getName())),
                            text(format("Time: {}", saveDate)),
                            text(format("      {}", saveTime)),
                        }),
                    });
                } else {
                    element = vbox({
                        text(format("Slot No.{}", index + 1)),
                    });
                }

                if (index == focusIndex) {
                    element |= focus;
                }
                return element;
            }
        );
        saveListContainer->Add(saveItem);
        saveListContainer->Add(
            Container::Horizontal({
                loadButton,
                saveButton,
                deleteButton
            }) | CatchEvent([&](Event event) {
                if (event.is_mouse()) {
                    if (event.mouse().button == Mouse::Left &&
                        event.mouse().motion == Mouse::Released) {
                        // Move focus to the container of the buttons.
                        itemIndex = index * 2;
                        return true;
                    }
                }
                return false;
            })
        );
    }
    saveListContainer |= CatchEvent([&](Event event) {
        if (event.is_mouse()) {
            if (event.mouse().button == Mouse::WheelUp) {
                focusIndex = max(0, focusIndex - 1);
                return true;
            }
            if (event.mouse().button == Mouse::WheelDown) {
                focusIndex = min(static_cast<int>(_saveList.size()) - 1, focusIndex + 1);
                return true;
            }
        }
        return false;
    });
    const auto dataRenderer = Renderer(
        saveListContainer,
        [&] {
            return window(
                text("Data Panel") | hcenter | bold,
                saveListContainer->Render() | vscroll_indicator | frame | flex
            );
        }
    );

    auto screen = ScreenInteractive::FitComponent();
    thread([&] {
        while (true) {
            this_thread::sleep_for(chrono::nanoseconds(16666666));
            screen.PostEvent(Event::Custom);
        }
    }).detach();
    screen.Loop(Container::Horizontal(
        {
            controlRenderer | size(WIDTH, EQUAL, controlPanelWidth),
            cellEventHandler | flex_grow | border,
            dataRenderer | size(WIDTH, EQUAL, dataPanelWidth)
        },
        &_panelIndex
    ));
}

Component GameOfLife::_createCellRenderer() {
    return Renderer([this] {
        const auto field = _fieldManager.getField();
        const auto width = static_cast<int>(field.size());
        const auto height = static_cast<int>(field[0].size());
        const auto mouse = _mouse.load();
        auto c = Canvas(width * 4, height * 4);
        for (const auto& [x,y]: views::cartesian_product(views::iota(0, width), views::iota(0, height))) {
            drawCell(
                c,
                {x, y},
                x == mouse.x && y == mouse.y
                    ? Color{Color::Grey50}
                    : field[x][y]
                          ? Color{Color::White}
                          : Color{Color::Grey11}
            );
        }

        // // The IDE doesn't like this, but this should compile and run without any issue.
        // // Related issue: [CPP-36762](https://youtrack.jetbrains.com/issue/CPP-36762)
        // ranges::for_each(
        //     ranges::views::cartesian_product(
        //         ranges::views::iota(0, _fieldSize.x),
        //         ranges::views::iota(0, _fieldSize.y)
        //     ),
        //     [&](const auto& pair) {
        //         const auto& [x, y] = pair;
        //         drawCell(
        //             c,
        //             {x, y},
        //             x == mouse.x && y == mouse.y
        //                 ? Color{Color::Grey50}
        //                 : field[x][y]
        //                       ? Color{Color::White}
        //                       : Color{Color::Grey11}
        //         );
        //     }
        // );
        return canvas(move(c));
    }) | CatchEvent([this](Event event) {
        if (event == Event::ArrowRight) {
            _fieldManager.isPlaying.store(false);
            _fieldManager.nextIteration();
        } else if (event == Event::ArrowLeft) {
            _fieldManager.isPlaying.store(false);
            _fieldManager.previousIteration();
        } else if (event.is_character()) {
            _handleNormalKeysEvent(event.character());
        } else if (event.is_mouse()) {
            _handleMouseEvent(event.mouse());
        }
        return false;
    });
}


Component GameOfLife::_createControlButtonContainer() {
    auto previousButton = component::makeButton(
        "<-",
        [this] {
            _fieldManager.isPlaying.store(false);
            _fieldManager.previousIteration();
        },
        colorLightBlue
    );

    auto playPauseButton = component::makeButton(
        [this] { return _fieldManager.isPlaying.load() ? "Pause" : "Start"; },
        [this] {
            _fieldManager.isPlaying.store(!_fieldManager.isPlaying.load());
        },
        colorGreen
    );

    auto nextButton = component::makeButton(
        "->",
        [this] {
            _fieldManager.isPlaying.store(false);
            _fieldManager.nextIteration();
        },
        colorLightBlue
    );

    auto randomizeButton = component::makeButton(
        "Randomize",
        [this] {
            _fieldManager.isPlaying.store(false);
            _fieldManager.randomize();
        },
        colorOrangeLight,
        colorOrange
    );

    auto resetButton = component::makeButton(
        "Reset",
        [this] {
            _fieldManager.isPlaying.store(false);
            _fieldManager.reset();
        },
        colorRedLight,
        colorNegative
    );

    return Container::Vertical({
        Container::Horizontal({move(previousButton), move(playPauseButton), move(nextButton)}),
        Container::Horizontal({move(randomizeButton), move(resetButton)})
    });
}

tuple<Component, Component>
GameOfLife::_createFieldSizeInputs(string& heightString, string& widthString) {
    using namespace ftxui;

    auto widthInput = Input(&widthString)
                      | CatchEvent([&](const Event& event) {
                          return event.is_character() && !isdigit(event.character()[0]) && widthString.length() < 2;
                      })
                      | CatchEvent([&](const Event& event) {
                          if (event == Event::Return) {
                              if (const auto width = widthString.empty() ? 0 : stoi(widthString);
                                  width > 0 && width != _fieldSize.x && width <= 28) {
                                  _fieldSize.x = width;
                                  _fieldManager.setSize(_fieldSize);
                              } else {
                                  widthString = to_string(_fieldSize.x);
                              }
                              _panelIndex = 1;
                              return true;
                          }
                          return false;
                      });
    auto heightInput = Input(&heightString)
                       | CatchEvent([&](const Event& event) {
                           return event.is_character() && !isdigit(event.character()[0]) && heightString.length() < 2;
                       })
                       | CatchEvent([&](const Event& event) {
                           if (event == Event::Return) {
                               if (const auto height = heightString.empty() ? 0 : stoi(heightString);
                                   height > 0 && height != _fieldSize.y && height <= 28) {
                                   _fieldSize.y = height;
                                   _fieldManager.setSize(_fieldSize);
                               } else {
                                   heightString = to_string(_fieldSize.x);
                               }
                               _panelIndex = 1;
                               return true;
                           }
                           return false;
                       });

    return {move(heightInput), move(widthInput)};
}

Component GameOfLife::_createPlaybackIntervalInput(string& playbackIntervalString) {
    using namespace ftxui;

    return Input(&playbackIntervalString)
           | CatchEvent([&](const Event& event) {
               return event.is_character() && !isdigit(event.character()[0]) && playbackIntervalString.length() < 4;
           })
           | CatchEvent([&](const Event& event) {
               if (event == Event::Return) {
                   if (const auto playbackInterval = playbackIntervalString.empty() ? 0 : stoi(playbackIntervalString);
                       playbackInterval >= 10 && playbackInterval <= 5000) {
                       _fieldManager.playbackInterval.store(chrono::milliseconds(playbackInterval));
                   } else {
                       playbackIntervalString = to_string(_fieldManager.playbackInterval.load().count());
                   }
                   _panelIndex = 1;
                   return true;
               }
               return false;
           });
}

void GameOfLife::_handleNormalKeysEvent(const string& keys) {
    switch (keys[0]) {
        case 'c': {
            _fieldManager.isPlaying.store(false);
            _fieldManager.reset();
            break;
        }
        case 'r': {
            _fieldManager.isPlaying.store(false);
            _fieldManager.randomize();
            break;
        }
        case ' ': {
            _fieldManager.isPlaying.store(!_fieldManager.isPlaying.load());
        }
        default: {
            break;
        }
    }
}

void GameOfLife::_handleMouseEvent(const Mouse& mouseEvent) {
    const auto& [button, motion, shift, meta, control, x, y] = mouseEvent;
    // Minus 1 for the border.
    const auto mouseInCanvas = Point{(x - controlPanelWidth - 1) / 2, y - 1};
    if (mouseInCanvas.x >= 0 && mouseInCanvas.x < _fieldSize.x
        && mouseInCanvas.y >= 0 && mouseInCanvas.y < _fieldSize.y) {
        _panelIndex = 1;
        switch (button) {
            case Mouse::Left: {
                _fieldManager.setCell(mouseInCanvas, true);
                break;
            }
            case Mouse::Right: {
                _fieldManager.setCell(mouseInCanvas, false);
                break;
            }
            default: {
                break;
            }
        }
    }
    _mouse.store(mouseInCanvas);
}
