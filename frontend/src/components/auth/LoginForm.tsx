import React, { useState } from 'react';
import { useAuth } from '../../context/AuthContext';
import authService from '../../services/auth.service';
import { useNavigate } from 'react-router-dom';
import { toast } from 'react-hot-toast';

interface LoginFormProps {
  onSwitch: () => void;
}

const LoginForm: React.FC<LoginFormProps> = ({ onSwitch }) => {
  const [email, setEmail] = useState('');
  const [password, setPassword] = useState('');
  const [isLoading, setIsLoading] = useState(false);
  const { login } = useAuth();
  const navigate = useNavigate();

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setIsLoading(true);
    try {
      console.log("Attempting login with email:", email);
      const response = await authService.login({ email, password });
      console.log("Login response:", response);
      
      if (response.success) {
        // Map backend response to User type
        const userData = {
          id: response.data.userId || response.data.id,
          userId: response.data.userId,
          email: response.data.email,
          fullName: response.data.fullName,
          role: response.data.role?.toUpperCase(),
          isActive: true,
          createdAt: new Date().toISOString(),
          updatedAt: new Date().toISOString()
        };
        
        login(response.data.token, userData as any);
        toast.success("Welcome back!");
        navigate('/dashboard');
      } else {
        const errorMsg = typeof response.error === 'object' 
          ? response.error.message || "Login failed" 
          : String(response.error || "Login failed");
        toast.error(errorMsg);
      }
    } catch (error: any) {
      console.error("Login error:", error);
      const errorMsg = error?.message || (typeof error === 'string' ? error : "An unexpected error occurred");
      toast.error(String(errorMsg));
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="p-8">
      <div className="text-center mb-8">
        <h2 className="text-3xl font-bold bg-gradient-to-r from-blue-400 to-purple-400 bg-clip-text text-transparent">
          Welcome Back
        </h2>
        <p className="text-slate-400 mt-2">Sign in to your account</p>
      </div>

      <form onSubmit={handleSubmit} className="space-y-5">
        <div>
          <label className="block text-sm font-medium text-slate-300 mb-1">Email address</label>
          <input
            type="email"
            value={email}
            onChange={(e) => setEmail(e.target.value)}
            className="w-full bg-white/5 border border-white/10 rounded-xl px-4 py-3 text-white placeholder-slate-500 focus:border-blue-500 transition"
            placeholder="example@gmail.com"
            required
          />
        </div>
        <div>
          <label className="block text-sm font-medium text-slate-300 mb-1">Password</label>
          <input
            type="password"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            className="w-full bg-white/5 border border-white/10 rounded-xl px-4 py-3 text-white placeholder-slate-500 focus:border-blue-500 transition"
            placeholder="········"
            required
          />
        </div>
        <div className="flex items-center justify-between">
          <label className="flex items-center gap-2 text-sm text-slate-300">
            <input type="checkbox" className="rounded bg-white/5 border-white/20" /> Remember Me
          </label>
          <a href="#" className="text-sm text-blue-400 hover:underline">Forgot Password?</a>
        </div>
        <button
          type="submit"
          disabled={isLoading}
          className="btn-primary w-full py-3 text-lg font-semibold"
        >
          {isLoading ? 'Logging in...' : 'Login'}
        </button>
      </form>

      <div className="mt-6 text-center">
        <p className="text-slate-400">
          Are you a new member?{' '}
          <button
            type="button"
            onClick={onSwitch}
            className="text-blue-400 hover:underline font-semibold"
          >
            Sign Up
          </button>
        </p>
      </div>
    </div>
  );
};

export default LoginForm;
