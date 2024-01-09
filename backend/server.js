require("dotenv").config();
// require('express-async-errors');
const express = require("express");
const app = express();
const cors = require("cors");
const corsOptions = require("./config/corsOptions");
// const connectDB = require("./config/dbConn");
// const mongoose = require("mongoose");
const mqtt = require('mqtt');

const PORT = process.env.PORT || 5001;


// connectDB();
// app.use(cors(corsOptions));
app.use(cors());
app.use(express.json());

// app.use('/users', require('./routes/userRoutes'));
const mqttRoutes = require('./routes/mqttRoutes');


// app.use(errorHandler);

// routes
// const subscriberRouter = require("./routes/subscriberRoutes");
// const publisherRouter = require("./routes/publisherRoutes");

  
// app.use("/subscriber", subscriberRouter);
// app.use("/publisher", publisherRouter);
app.use('/mqtt', mqttRoutes);

// mongoose.set('strictQuery', false);
// mongoose.connection.once("open", () => {
//   console.log("MongoDB connected");
//   app.listen(PORT, () => console.log(`Server listening on port ${PORT}`));
// });

// mongoose.connection.on("error", (err) => {
//     console.log(`MongoDB connection error: ${err}`);
//     logEvents(`${err.no}: ${err.code}\t${err.syscall}\t${err.hostname}`, "mongoErrLog.log");
//   });

app.listen(PORT, '0.0.0.0', () => console.log(`Server listening on port ${PORT}`));
