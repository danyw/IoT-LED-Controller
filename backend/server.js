require("dotenv").config();
// require('express-async-errors');
const express = require("express");
const app = express();
const cors = require("cors");
const corsOptions = require("./config/corsOptions");
const connectDB = require("./config/dbConn");
const mongoose = require("mongoose");
const mqtt = require('mqtt');
const cookieParser = require("cookie-parser");

const PORT = process.env.PORT || 5001;



app.use(cors(corsOptions));
app.use(cors());
app.use(express.json());
app.use(cookieParser());
connectDB();
// app.use('/users', require('./routes/userRoutes'));
const mqttRoutes = require('./routes/mqttRoutes');
const userRoutes = require('./routes/userRoutes');
const authRoutes = require('./routes/authRoutes');



// app.use(errorHandler);

// routes
// const subscriberRouter = require("./routes/subscriberRoutes");
// const publisherRouter = require("./routes/publisherRoutes");

  
// app.use("/subscriber", subscriberRouter);
// app.use("/publisher", publisherRouter);
app.use('/mqtt', mqttRoutes);
app.use('/users', userRoutes);
app.use('/auth', authRoutes);

// mongoose.set('strictQuery', false);
// mongoose.connection.once("open", () => {
//   console.log("MongoDB connected");
//   app.listen(PORT, () => console.log(`Server listening on port ${PORT}`));
// });

// mongoose.connection.on("error", (err) => {
//     console.log(`MongoDB connection error: ${err}`);
//     logEvents(`${err.no}: ${err.code}\t${err.syscall}\t${err.hostname}`, "mongoErrLog.log");
//   });

mongoose.set('strictQuery', false);
mongoose.connection.once("open", () => {
  console.log("MongoDB connection established successfully");
  app.listen(PORT, '0.0.0.0', () => console.log(`Server listening on port ${PORT}`));
});

// app.listen(PORT, '0.0.0.0', () => console.log(`Server listening on port ${PORT}`));
