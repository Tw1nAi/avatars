- bear cool animations: eyes, ears, liking paws,slipping
- menus to handle camera position and light strength
- suggestions
- ! debounce or other mechanism to deal with multiple attempts to use mic
- exception handling that involves informing user of the error
- !! menu that lists all dialogs for selected avatars and allows to click through each one to check both dialog sound and animation
- hidden system menu that can be opened by typing specyfic phrase to text box
- runtime app logs in text files
- list of widgets that should be hidden from the start, stored in root widget
- add debug mode command via user text input
- hard save of app options and options per avatar!!!!
- once hard save to disk been made display notification, i.e. "Camera position saved". Put it in GameInstance subclass.
- split logic in AvatarsPlayerController to other classes: GameInstance (UI, options), GameMode, etc
- extract numeric input field from UConversationOptionsWidget
- option: UI scaling scrollbar
- bug: when dialog is already playing and second dialog is played the animations get mixed?

- [x] language selection
  - [x] ui texts change
  - [x] language buttons selected visual state change
- [ ] keyboard input...
- [x] animations reactions
   - [x] listening
   - [x] talking
- [x] dont' send empty message
- [ ] restart of face animation when the same animation is played twice

- [x] load sounds assets
- [x] load face anim assets
- [ ] UI
  - [x] loading screen
  - [ ] character selection
- [ ] character selection

- [x] turn off character removal of text after N seconds
- [ ] spróbuj wyciągnąć score 0 - 1 pewności transkrypcji
- [ ] przetestuj znaczki inne niż litery z modelem
  - [ ] zablokuj dodawanie znaków innych niż tekst i cyfry przez klawiaturę
- [ ] Move input logic to C++
- [ ] ui animations
 - [ ] loading screen