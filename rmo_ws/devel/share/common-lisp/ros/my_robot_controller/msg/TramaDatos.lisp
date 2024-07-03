; Auto-generated. Do not edit!


(cl:in-package my_robot_controller-msg)


;//! \htmlinclude TramaDatos.msg.html

(cl:defclass <TramaDatos> (roslisp-msg-protocol:ros-message)
  ((u_m
    :reader u_m
    :initarg :u_m
    :type (cl:vector cl:float)
   :initform (cl:make-array 4 :element-type 'cl:float :initial-element 0.0))
   (v_bat
    :reader v_bat
    :initarg :v_bat
    :type cl:fixnum
    :initform 0))
)

(cl:defclass TramaDatos (<TramaDatos>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <TramaDatos>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'TramaDatos)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name my_robot_controller-msg:<TramaDatos> is deprecated: use my_robot_controller-msg:TramaDatos instead.")))

(cl:ensure-generic-function 'u_m-val :lambda-list '(m))
(cl:defmethod u_m-val ((m <TramaDatos>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader my_robot_controller-msg:u_m-val is deprecated.  Use my_robot_controller-msg:u_m instead.")
  (u_m m))

(cl:ensure-generic-function 'v_bat-val :lambda-list '(m))
(cl:defmethod v_bat-val ((m <TramaDatos>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader my_robot_controller-msg:v_bat-val is deprecated.  Use my_robot_controller-msg:v_bat instead.")
  (v_bat m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <TramaDatos>) ostream)
  "Serializes a message object of type '<TramaDatos>"
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'u_m))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'v_bat)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'v_bat)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <TramaDatos>) istream)
  "Deserializes a message object of type '<TramaDatos>"
  (cl:setf (cl:slot-value msg 'u_m) (cl:make-array 4))
  (cl:let ((vals (cl:slot-value msg 'u_m)))
    (cl:dotimes (i 4)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits)))))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'v_bat)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'v_bat)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<TramaDatos>)))
  "Returns string type for a message object of type '<TramaDatos>"
  "my_robot_controller/TramaDatos")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'TramaDatos)))
  "Returns string type for a message object of type 'TramaDatos"
  "my_robot_controller/TramaDatos")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<TramaDatos>)))
  "Returns md5sum for a message object of type '<TramaDatos>"
  "fc2aa8d0931f2ca3f5ab9b875c7494e0")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'TramaDatos)))
  "Returns md5sum for a message object of type 'TramaDatos"
  "fc2aa8d0931f2ca3f5ab9b875c7494e0")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<TramaDatos>)))
  "Returns full string definition for message of type '<TramaDatos>"
  (cl:format cl:nil "float32[4] u_m~%uint16 v_bat~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'TramaDatos)))
  "Returns full string definition for message of type 'TramaDatos"
  (cl:format cl:nil "float32[4] u_m~%uint16 v_bat~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <TramaDatos>))
  (cl:+ 0
     0 (cl:reduce #'cl:+ (cl:slot-value msg 'u_m) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     2
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <TramaDatos>))
  "Converts a ROS message object to a list"
  (cl:list 'TramaDatos
    (cl:cons ':u_m (u_m msg))
    (cl:cons ':v_bat (v_bat msg))
))
