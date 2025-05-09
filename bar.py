import pygame
import random

# Initialize pygame
pygame.init()

# Screen dimensions
SCREEN_WIDTH = 400
SCREEN_HEIGHT = 300
BAR_Y = SCREEN_HEIGHT // 2
PLAYER_SIZE = 20

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)

# Screen setup
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
pygame.display.set_caption("Bar Game")

# Player settings
player = {"x": 10, "y": BAR_Y - PLAYER_SIZE // 2, "speed": 5, "on_top": True}
player_rect = pygame.Rect(player["x"], player["y"], PLAYER_SIZE, PLAYER_SIZE)

# Bar settings
bar_length = SCREEN_WIDTH - 20

# Obstacle settings
obstacles = []
obstacle_speed = 3
obstacle_spawn_rate = 30  # Frames per obstacle
score = 0
frame_count = 0

# Font settings
font = pygame.font.SysFont(None, 36)

def draw_bar():
    pygame.draw.line(screen, WHITE, (10, BAR_Y), (bar_length, BAR_Y), 2)

def draw_player():
    player_rect.y = player["y"]
    pygame.draw.rect(screen, RED, player_rect)

def draw_obstacles():
    for ob in obstacles:
        pygame.draw.rect(screen, WHITE, ob)

def check_collision():
    global running
    for ob in obstacles:
        if player_rect.colliderect(ob):
            running = False

def update_obstacles():
    global score, obstacle_speed
    for ob in obstacles:
        ob.x -= obstacle_speed
    obstacles[:] = [ob for ob in obstacles if ob.x > -20]
    if frame_count % obstacle_spawn_rate == 0:
        pos = random.choice([BAR_Y - 30, BAR_Y + 10])
        obstacles.append(pygame.Rect(SCREEN_WIDTH, pos, 20, 20))
        score += 1
        if score % 5 == 0:
            obstacle_speed += 1

def draw_score():
    score_text = font.render(f"Score: {score}", True, WHITE)
    screen.blit(score_text, (SCREEN_WIDTH - 150, 10))

# Game loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_SPACE:
                player["on_top"] = not player["on_top"]
                player["y"] = BAR_Y - PLAYER_SIZE if player["on_top"] else BAR_Y
            if event.key == pygame.K_q:
                running = False

    screen.fill(BLACK)
    draw_bar()
    draw_player()
    draw_obstacles()
    draw_score()
    check_collision()
    update_obstacles()

    pygame.display.flip()
    pygame.time.delay(30)
    frame_count += 1

pygame.quit()
