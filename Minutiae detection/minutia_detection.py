import cv2

# Marking minutiae on image
def draw_circle(event, x, y, flags, param):
    global tmp, stop
    if event == cv2.EVENT_LBUTTONDOWN:
        cv2.circle(img, (x, y), 3, (0, 0, 255), -1)
        cv2.imshow("image", img)
    if event == cv2.EVENT_RBUTTONDOWN:
        cv2.circle(img, (x, y), 3, (0, 0, 255), -1)
        tmp = (x, y)
        stop = True
    if event == cv2.EVENT_RBUTTONUP:
        cv2.line(img, (x,y), tmp,  (0, 0, 255), 2, cv2.LINE_AA)
        cv2.circle(img, (x, y), 3, (0, 0, 255), -1)
        stop = False
    if event == cv2.EVENT_MOUSEMOVE:
        flag = stop
        img2 = img.copy()
        if flag:
            cv2.line(img2, tmp, (x,y),  (0, 0, 255), 2, cv2.LINE_AA)
        cv2.imshow("image", img2)


def nothing(x):
    pass

# Load image
img = cv2.imread("fingerprint.jpg")
cv2.namedWindow('image')
cv2.setMouseCallback('image', draw_circle)

# create trackbars for scaling
cv2.createTrackbar('Scale', 'image', 1, 10, nothing)

# create switch for ON/OFF functionality
switch = '0 : Not Scaling \n1 : Scaling'
cv2.createTrackbar(switch, 'image', 0, 1, nothing)

scale = cv2.getTrackbarPos('Scale', 'image')

cv2.imshow('image', img)

while (1):
    scale = cv2.getTrackbarPos('Scale', 'image')
    sw = cv2.getTrackbarPos(switch, 'image')
    if sw == 1:
        img = cv2.resize(img, None, fx=(1 + scale/10.0), fy=(1 + scale/10.0), interpolation=cv2.INTER_CUBIC)
        cv2.setTrackbarPos(switch, 'image', 0)
        cv2.imshow("image", img)
    if cv2.waitKey(20) & 0xFF == 27:
        break
cv2.destroyAllWindows()