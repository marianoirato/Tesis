
(cl:in-package :asdf)

(defsystem "my_robot_controller-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "TramaDatos" :depends-on ("_package_TramaDatos"))
    (:file "_package_TramaDatos" :depends-on ("_package"))
  ))