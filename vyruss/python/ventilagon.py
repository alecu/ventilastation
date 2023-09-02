import povdisplay
from director import director
from scene import Scene


class VentilagonGame(Scene):
    def on_enter(self):
        povdisplay.ventilagon_enter()

    def on_exit(self):
        povdisplay.ventilagon_exit()

    def step(self):
        if director.was_pressed(director.BUTTON_A):
            pass

        if director.was_pressed(director.BUTTON_D):
            director.pop()
            raise StopIteration()
