const mongoose = require("mongoose");


const userProfileSchema = new mongoose.Schema({
    user: {
        type: mongoose.Schema.Types.ObjectId,
        required: true,
        ref: "User",
    },
    profileName: {
        type: String,
        required: true,
        unique: true
    },
    time: {
        type: Date,
        required: true,
        default: Date.now
    },
    pwmSettings: {
        red: Number,
        green: Number,
        blue: Number,
        coolWhite: Number,
        warmWhite: Number
    }
});

const UserProfile = mongoose.model("UserProfile", userProfileSchema);
