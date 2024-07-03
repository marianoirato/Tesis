;; Auto-generated. Do not edit!


(when (boundp 'my_robot_controller::TramaDatos)
  (if (not (find-package "MY_ROBOT_CONTROLLER"))
    (make-package "MY_ROBOT_CONTROLLER"))
  (shadow 'TramaDatos (find-package "MY_ROBOT_CONTROLLER")))
(unless (find-package "MY_ROBOT_CONTROLLER::TRAMADATOS")
  (make-package "MY_ROBOT_CONTROLLER::TRAMADATOS"))

(in-package "ROS")
;;//! \htmlinclude TramaDatos.msg.html


(defclass my_robot_controller::TramaDatos
  :super ros::object
  :slots (_u_m _v_bat ))

(defmethod my_robot_controller::TramaDatos
  (:init
   (&key
    ((:u_m __u_m) (make-array 4 :initial-element 0.0 :element-type :float))
    ((:v_bat __v_bat) 0)
    )
   (send-super :init)
   (setq _u_m __u_m)
   (setq _v_bat (round __v_bat))
   self)
  (:u_m
   (&optional __u_m)
   (if __u_m (setq _u_m __u_m)) _u_m)
  (:v_bat
   (&optional __v_bat)
   (if __v_bat (setq _v_bat __v_bat)) _v_bat)
  (:serialization-length
   ()
   (+
    ;; float32[4] _u_m
    (* 4    4)
    ;; uint16 _v_bat
    2
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32[4] _u_m
     (dotimes (i 4)
       (sys::poke (elt _u_m i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; uint16 _v_bat
       (write-word _v_bat s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32[4] _u_m
   (dotimes (i (length _u_m))
     (setf (elt _u_m i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     )
   ;; uint16 _v_bat
     (setq _v_bat (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;;
   self)
  )

(setf (get my_robot_controller::TramaDatos :md5sum-) "fc2aa8d0931f2ca3f5ab9b875c7494e0")
(setf (get my_robot_controller::TramaDatos :datatype-) "my_robot_controller/TramaDatos")
(setf (get my_robot_controller::TramaDatos :definition-)
      "float32[4] u_m
uint16 v_bat


")



(provide :my_robot_controller/TramaDatos "fc2aa8d0931f2ca3f5ab9b875c7494e0")


