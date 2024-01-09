
import useAuth from "../../hooks/useAuth";
import { login, logout, refreshToken } from "./authService"

const handleSubmit = async (e) => {
    e.preventDefault();
    try {
      const accessToken = await AuthService.login(username, password);
      auth.login({ username, accessToken }); // Update the auth context
      // Redirect to dashboard or other page
    } catch (err) {
      // Handle error
    }
  };
  