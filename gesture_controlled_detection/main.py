import math
import time

import cv2
import mediapipe as mp
import serial

def distance (point1, point2) :
    return math.sqrt (math.pow (point1[0] - point2[0], 2) + math.pow(point1[1] - point2[1], 2))

def is_fist (landmarks) :
    lms = enumerate (landmarks)
    lm = next (lms)

    point_0 = [lm[1].x, lm[1].y]
    lm = next (lms)
    lm = next (lms)

    point_2 = [lm[1].x, lm[1].y]
    lm = next (lms)
    lm = next (lms)

    point_4 = [lm[1].x, lm[1].y]
    lm = next (lms)

    point_5 = [lm[1].x, lm[1].y]
    lm = next (lms)
    lm = next (lms)
    lm = next (lms)

    point_8 = [lm[1].x, lm[1].y]
    lm = next (lms)

    point_9 = [lm[1].x, lm[1].y]
    lm = next (lms)
    lm = next (lms)
    lm = next (lms)

    point_12 = [lm[1].x, lm[1].y]
    lm = next (lms)

    point_13 = [lm[1].x, lm[1].y]
    lm = next (lms)
    lm = next (lms)
    lm = next (lms)

    point_16 = [lm[1].x, lm[1].y]
    lm = next (lms)

    point_17 = [lm[1].x, lm[1].y]
    lm = next (lms)
    lm = next (lms)
    lm = next (lms)

    point_20 = [lm[1].x, lm[1].y]
    

    # Check 4 is closer to 17 than 2
    dist4_17 = distance (point_4, point_17)
    dist2_17 = distance (point_2, point_17)

    if (dist2_17 < dist4_17) :
        return False

    # Check that 8 is closer to 0 than 5
    dist8_0 = distance (point_8, point_0)
    dist5_0 = distance (point_5, point_0)

    if (dist5_0 < dist8_0) :
        return False

    # Check that 12 is closer to 0 than 9
    dist12_0 = distance (point_12, point_0)
    dist9_0 = distance (point_9, point_0)

    if (dist9_0 < dist12_0) :
        return False

    # Check that 16 is closer to 0 than 13
    dist16_0 = distance (point_16, point_0)
    dist13_0 = distance (point_13, point_0)

    if (dist13_0 < dist16_0) :
        return False

    # Check that 20 is closer to 0 than 17
    dist20_0 = distance (point_20, point_0)
    dist17_0 = distance (point_17, point_0)
    
    if (dist17_0 < dist20_0) :
        return False
    
    return True

def check_quad (landmarks) :
    f1 = lambda x : -1 * x + 1
    f2 = lambda x : x 
    quads = [0, 0, 0, 0]
    dead_center = 0

    for lm in landmarks :
        if lm.x >= 0.45 and lm.x <= 0.55 and lm.y >= 0.4 and lm.y <= 0.6 :
            dead_center += 1 # Inside STOP circle

        if lm.y < f1(lm.x) and lm.y < f2(lm.x) :
            quads[2] += 1 # Top quadrant

        elif lm.y > f1(lm.x) and lm.y > f2(lm.x) :
            quads[3] += 1 # Bottom quadrant

        elif lm.y < f1(lm.x) and lm.y > f2(lm.x) :
            quads[1] += 1 # Right quadrant

        elif lm.y > f1(lm.x) and lm.y < f2(lm.x) :
            quads[0] += 1 # Left quadrant

    most = 0
    num = 0
    i = 0
    for quad in quads :
        if (quad > num) :
            num = quad
            most = i
        i += 1

    return most if most >= dead_center else -1

def is_in_middle (landmarks) :
    inside = 0
    outside = 0
    
    for lm in landmarks :
        if lm.x >= 0.36 and lm.x <= 0.64 and lm.y >= 0.28 and lm.y <= 0.72 :
            inside += 1
        else : 
            outside += 1

    return True if inside >= outside else False




ser = serial.Serial ('COM6', 9600)
vid = cv2.VideoCapture (0) # 1 for external webcam
vid.set(3, 960)

mphands = mp.solutions.hands
mpdraw = mp.solutions.drawing_utils
Hands = mphands.Hands (max_num_hands = 1, min_detection_confidence = 0.7, min_tracking_confidence = 0.6)

action_taken = False
dir = -1

while True :
    success, frame = vid.read ()

    # Convert bgr to rgb
    RGBframe = cv2.cvtColor (frame, cv2.COLOR_BGR2RGB)
    result = Hands.process (RGBframe)

    h = math.floor (frame.shape[0] * 0.5)
    w = math.floor (frame.shape[1] * 0.5)

    cv2.circle (frame, (w, h), 125, (255, 255, 0))

    # cv2.line (frame, (0, 0), (frame.shape[1], frame.shape[0]), (255, 255, 0))
    cv2.line (frame, (0, 0), (371, 210), (255, 255, 0))
    cv2.line (frame, (589, 334), (frame.shape[1], frame.shape[0]), (255, 255, 0))

    # cv2.line (frame, (0, frame.shape[0]), (frame.shape[1], 0), (255, 255, 0))
    cv2.line (frame, (0, frame.shape[0]), (371, 333), (255, 255, 0))
    cv2.line (frame, (589, 211), (frame.shape[1], 0), (255, 255, 0))
    
    if result.multi_hand_landmarks :
        for handLm in result.multi_hand_landmarks :
            mpdraw.draw_landmarks (frame, handLm, mphands.HAND_CONNECTIONS)

            fist = is_fist (handLm.landmark)
            if not action_taken :
                dir = check_quad (handLm.landmark)
                action_taken = True
            
            if fist :
                centered = is_in_middle (handLm.landmark)

                if centered :
                    dir = -1
                    action_taken = False
                    
                else :
                    if dir == 0 or dir == 4 :
                        dir = 4
                    elif dir == 1 or dir == 5 :
                        dir = 5
                    # else :
                        # dir = -1

            print ("Strafe Left" if dir == 0 else ("Strafe Right" if dir == 1 else ("Forward" if dir == 2 else ("Backward" if dir == 3 else ("Rotate Left" if dir == 4 else ("Rotate Right" if dir == 5 else "STOP"))))))
            command = b'a' if dir == 0 else (b'd' if dir == 1 else (b'w' if dir == 2 else (b's' if dir == 3 else (b'q' if dir == 4 else (b'e' if dir == 5 else b'f')))))
            ser.write (command)


    frame = cv2.flip (frame, 1)
    cv2.imshow ("video", frame)
    cv2.waitKey (1)

ser.close ()