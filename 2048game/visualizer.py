from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from solver import Solver
from game import Game
import time
driver = webdriver.Chrome()
driver.get("https://play2048.co/")
html_elem = driver.find_element_by_tag_name('html')

TIME_INTERVAL = 1.5


def get_board():
    board = []
    for i in range(4):
        row = []
        for j in range(4):
            title_container_elem = driver.find_element_by_class_name(
                'tile-container')
            tile = title_container_elem.find_elements_by_class_name(
                f'tile-position-{j+1}-{i+1}')
            if(len(tile) == 0):
                val = 0
            elif (tile[0].text == ""):
                val = 0
            elif (len(tile) == 3):
                val = int(tile[2].text)
            else:
                val = int(tile[0].text)
            row.append(val)
        board.append(row)
    return board


res = Solver()


def send_allows():
    while(True):
        time.sleep(1)
        board = get_board()
        game = Game(board)
        direction = res.solve(get_board())
        if direction == 'u':
            html_elem.send_keys(Keys.UP)
        if direction == 'l':
            html_elem.send_keys(Keys.LEFT)
        if direction == 'd':
            html_elem.send_keys(Keys.DOWN)
        if direction == 'r':
            html_elem.send_keys(Keys.RIGHT)
    driver.close()
    driver.quit()


time.sleep(1)
send_allows()