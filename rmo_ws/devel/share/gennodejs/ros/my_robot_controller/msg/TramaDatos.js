// Auto-generated. Do not edit!

// (in-package my_robot_controller.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class TramaDatos {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.u_m = null;
      this.v_bat = null;
    }
    else {
      if (initObj.hasOwnProperty('u_m')) {
        this.u_m = initObj.u_m
      }
      else {
        this.u_m = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('v_bat')) {
        this.v_bat = initObj.v_bat
      }
      else {
        this.v_bat = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type TramaDatos
    // Check that the constant length array field [u_m] has the right length
    if (obj.u_m.length !== 4) {
      throw new Error('Unable to serialize array field u_m - length must be 4')
    }
    // Serialize message field [u_m]
    bufferOffset = _arraySerializer.float32(obj.u_m, buffer, bufferOffset, 4);
    // Serialize message field [v_bat]
    bufferOffset = _serializer.uint16(obj.v_bat, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type TramaDatos
    let len;
    let data = new TramaDatos(null);
    // Deserialize message field [u_m]
    data.u_m = _arrayDeserializer.float32(buffer, bufferOffset, 4)
    // Deserialize message field [v_bat]
    data.v_bat = _deserializer.uint16(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 18;
  }

  static datatype() {
    // Returns string type for a message object
    return 'my_robot_controller/TramaDatos';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'fc2aa8d0931f2ca3f5ab9b875c7494e0';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    float32[4] u_m
    uint16 v_bat
    
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new TramaDatos(null);
    if (msg.u_m !== undefined) {
      resolved.u_m = msg.u_m;
    }
    else {
      resolved.u_m = new Array(4).fill(0)
    }

    if (msg.v_bat !== undefined) {
      resolved.v_bat = msg.v_bat;
    }
    else {
      resolved.v_bat = 0
    }

    return resolved;
    }
};

module.exports = TramaDatos;
