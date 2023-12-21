#include <ranges>

#include <ftxui/component/screen_interactive.hpp>

#include <helpers/ButtonHelper.h>
#include <helpers/ColorHelper.h>
#include <types/common.h>
#include <types/GameOfLife.h>
#include <utils/console.h>

using namespace ftxui;
using namespace helpers;
using namespace std;
using namespace types;
using namespace utils;

namespace {
    constexpr auto cell = "██";
    constexpr auto controlPanelWidth = 25;
    constexpr auto inputWidth = 8;

    const auto controlValueSize = size(WIDTH, EQUAL, inputWidth);

    void drawCell(Canvas& c, const Point& mouse, const Color color) {
        c.DrawText(4 * mouse.x, 4 * mouse.y, cell, color);
    }
}

GameOfLife::GameOfLife(const int height, const int width)
    : _fieldManager(height, width), _fieldSize(width, height) {
    console::setConsoleSize(height + 2, width + 2);
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
    const auto [
        previousIterationButton,
        togglePlayPauseButton,
        nextIterationButton
    ] = _createControlButtons();

    const auto saveListContainer = Container::Vertical({});
    for (int index = 0; index < _saveList.size(); ++index) {
        const auto [
            loadButton,
            saveButton,
            deleteButton
        ] = _createSaveButtons(index);
        const auto saveInfo = Renderer([&, this, index] {
            const auto [saveDate, saveTime] = _saveList[index].getSaveTime();
            return vbox({
                text(format("Name: {}", _saveList[index].getName())),
                text(format("Time: {}", saveDate)),
                text(saveTime) | align_right,
            });
        });
        const auto isValid = !_saveList[index].load().empty();
        const auto container = Container::Vertical({
            saveInfo | Maybe(&isValid),
            Container::Horizontal({
                loadButton | Maybe(&isValid),
                saveButton,
                deleteButton | Maybe(&isValid)
            }) | align_right,
        });
        const auto saveItem = Renderer(
            container,
            [&, this, index] {
                return vbox({
                    text(format("Slot No.{}", index + 1)),
                    container->Render() | flex,
                    separator()
                });
            }
        );
        saveListContainer->Add(saveItem);
    }

    auto randomizeButton = ButtonHelper(
        "Randomize",
        [this] { _fieldManager.randomize(); },
        colorOrangeLight,
        colorOrange
    );

    auto resetButton = ButtonHelper(
        "Reset",
        [this] { _fieldManager.reset(); },
        colorRedLight,
        colorNegative
    );

    const auto controlRenderer = Renderer(
        Container::Vertical({
            widthInput,
            heightInput,
            saveListContainer,
            playbackIntervalInput,
            Container::Horizontal({previousIterationButton, togglePlayPauseButton, nextIterationButton}),
            Container::Horizontal({randomizeButton.component(), resetButton.component()})
        }),
        [&] {
            const auto aliveCount = _fieldManager.getCellCount();
            return vbox({
                text("Control Panel") | hcenter | bold,
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
                saveListContainer->Render() | vscroll_indicator | frame | flex,
                separator(),
                hbox({
                    text("(ms)Interval: ") | align_right | flex,
                    playbackIntervalInput->Render() | controlValueSize,
                }),
                hbox({
                    previousIterationButton->Render(),
                    togglePlayPauseButton->Render() | flex,
                    nextIterationButton->Render(),
                }),
                hbox({randomizeButton.render() | flex, resetButton.render()}),
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
            });
        }
    );

    const auto cellEventHandler = _createCellRenderer();

    const auto pageRenderer = Renderer(
        Container::Horizontal({controlRenderer, cellEventHandler}, &_panelIndex),
        [&] {
            return hflow({
                controlRenderer->Render() | size(WIDTH, EQUAL, controlPanelWidth) | flex | border,
                cellEventHandler->Render() | border
            });
        }
    );

    auto screen = ScreenInteractive::FitComponent();
    thread([&] {
        while (true) {
            this_thread::sleep_for(chrono::nanoseconds(16666666));
            screen.PostEvent(Event::Custom);
        }
    }).detach();
    screen.Loop(pageRenderer);
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

        // // The IDE doesn't like this, but it compiles and works.
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
        //             x == _mouse.x && y == _mouse.y
        //                 ? Color{Color::Grey50}
        //                 : _field[x][y]
        //                       ? Color{Color::White}
        //                       : Color{Color::Grey11}
        //         );
        //     }
        // );
        return canvas(move(c));
    }) | CatchEvent([this](Event event) {
        if (event == Event::ArrowRight) {
            _fieldManager.nextIteration();
        } else if (event == Event::ArrowLeft) {
            _fieldManager.previousIteration();
        } else if (event.is_character()) {
            _handleNormalKeysEvent(event.character());
        } else if (event.is_mouse()) {
            _handleMouseEvent(event.mouse());
        }
        return true;
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

tuple<Component, Component, Component> GameOfLife::_createControlButtons() {
    auto previousIterationButton = Button(
        "<-",
        [this] {
            _fieldManager.isPlaying.store(false);
            _fieldManager.previousIteration();
        },
        [this] {
            ButtonOption option;
            option.transform = [this](const EntryState& s) {
                auto element = text(s.label) | center | border;
                if (s.active) {
                    element |= bold;
                }
                return element;
            };
            option.animated_colors.foreground.Set(Color::White, Color::DeepSkyBlue1);
            option.animated_colors.background.Set(Color::Default, Color::Default);
            return option;
        }()
    );
    auto togglePlayPauseButton = Button(
        "Start",
        [this] {
            _fieldManager.isPlaying.store(!_fieldManager.isPlaying.load());
        },
        [this] {
            ButtonOption option;
            option.transform = [this](const EntryState& s) {
                auto element = text(_fieldManager.isPlaying.load() ? "Pause" : "Start") | center | border;
                if (s.active) {
                    element |= bold;
                }
                return element;
            };
            option.animated_colors.foreground.Set(Color::White, Color::LightGreen);
            option.animated_colors.background.Set(Color::Default, Color::Default);
            return option;
        }()
    );
    auto nextIterationButton = Button(
        "->",
        [this] {
            _fieldManager.isPlaying.store(false);
            _fieldManager.nextIteration();
        },
        [this] {
            ButtonOption option;
            option.transform = [this](const EntryState& s) {
                auto element = text(s.label) | center | border;
                if (s.active) {
                    element |= bold;
                }
                return element;
            };
            option.animated_colors.foreground.Set(Color::White, Color::DeepSkyBlue1);
            option.animated_colors.background.Set(Color::Default, Color::Default);
            return option;
        }()
    );
    return {move(previousIterationButton), move(togglePlayPauseButton), move(nextIterationButton)};
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

tuple<Component, Component, Component> GameOfLife::_createSaveButtons(const int index) {
    auto loadButton = Button(
        "Load",
        [this, index] {
            _fieldManager.setField(_saveList[index].load());
        },
        [this] {
            ButtonOption option;
            option.transform = [this](const EntryState& s) {
                auto element = text(s.label) | center | border;
                if (s.active) {
                    element |= bold;
                }
                return element;
            };
            option.animated_colors.foreground.Set(Color::White, Color::DeepSkyBlue1);
            option.animated_colors.background.Set(Color::Default, Color::Default);
            return option;
        }()
    );
    auto saveButton = Button(
        "Save",
        [this, index] {
            _saveList[index].save(_fieldManager.getField());
        },
        [this] {
            ButtonOption option;
            option.transform = [this](const EntryState& s) {
                auto element = text(s.label) | center | border;
                if (s.active) {
                    element |= bold;
                }
                return element;
            };
            option.animated_colors.foreground.Set(Color::White, Color::DeepSkyBlue1);
            option.animated_colors.background.Set(Color::Default, Color::Default);
            return option;
        }()
    );
    auto deleteButton = Button(
        "Delete",
        [this, index] {
            _saveList.erase(_saveList.begin() + index);
        },
        [this] {
            ButtonOption option;
            option.transform = [this](const EntryState& s) {
                auto element = text(s.label) | center | border;
                if (s.active) {
                    element |= bold;
                }
                return element;
            };
            option.animated_colors.foreground.Set(Color::White, Color::RedLight);
            option.animated_colors.background.Set(Color::Default, Color::Default);
            return option;
        }()
    );
    return {move(loadButton), move(saveButton), move(deleteButton)};
}

void GameOfLife::_handleNormalKeysEvent(const string& keys) {
    switch (keys[0]) {
        case 'c': {
            _fieldManager.reset();
            break;
        }
        case 'r': {
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
    const auto mouseInCanvas = Point{(x - controlPanelWidth - 3) / 2, y - 1};
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
