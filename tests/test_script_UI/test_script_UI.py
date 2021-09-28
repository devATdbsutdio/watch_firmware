# -- GLOBAL IMPORTS -- #
import threading
import curses
import sys
import npyscreen
import os

npyscreen.disableColor()


# UI main function which is used to draw the terminal UI
def main():
    app = App()
    app.run()


class App(npyscreen.NPSApp):
    def main(self):
        form = npyscreen.FormBaseNew(name="WATCH HW SW TESTING UNIT")

        # Android device stat board
        general_info_panel = form.add(
            Column,
            name="GENERAL INFO",
            relx=2,
            rely=2,
            max_width=40,
            height=8
            #  max_height=terminal_dimensions()[0] - 10
        )

        # watch the form and update values
        while True:
            general_info_panel.values = [
                "CURR FIRMWARE:",
                "PULL STAT:",
                "TARGET UC:",
                "UPLOAD PORT:",
                "DEBUG PORT:",
                "TOTAL UPLOADS:"
            ]

            form.display()


class Column(npyscreen.BoxTitle):
    def resize(self):
        self.max_height = int(0.73 * terminal_dimensions()[0])


def terminal_dimensions():
    return curses.initscr().getmaxyx()   



if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(1)